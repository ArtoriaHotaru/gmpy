/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * gmpy_mpfr.h                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Python interface to the GMP or MPIR, MPFR, and MPC multiple precision   *
 * libraries.                                                              *
 *      Copyright 2000 - 2009 Alex Martelli                                *
 *      Copyright 2008 - 2011 Case Van Horsen                              *
 *                                                                         *
 * This library is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU Lesser General Public License as published   *
 * by the Free Software Foundation; either version 2.1 of the License, or  *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * Lesser General Public License for more details.                         *
 *                                                                         *
 * You should have received a copy of the GNU Lesser General Public        *
 * License along with this library; if not, write to the Free Software     *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA           *
 * 02110-1301  USA                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* gmpy_mpfr C API extension header file.
 *
 * Provide interface to the MPFR (Multiple Precision Floating-point with
 * Rounding) library.
 *
 * Version 2.00, April 2011 (created) casevh
 *
 * This file is expected to be included from gmpy.h
 */

#if !defined(FLT_RADIX) || (FLT_RADIX!=2)
#   error "FLT_RADIX undefined or != 2, GMPY2 is confused. :("
#endif

#if defined(MS_WIN32) && defined(_MSC_VER)
#  pragma comment(lib,"mpfr.lib")
#endif

typedef struct {
    mpob ob;
    mpfr_t f;
    Py_hash_t hash_cache;
    int rc;
    int round_mode;
} PympfrObject;

#define GMPY_DIVZERO(msg) PyErr_SetString(GMPyExc_DivZero, msg)
#define GMPY_INEXACT(msg) PyErr_SetString(GMPyExc_Inexact, msg)
#define GMPY_INVALID(msg) PyErr_SetString(GMPyExc_Invalid, msg)
#define GMPY_OVERFLOW(msg) PyErr_SetString(GMPyExc_Overflow, msg)
#define GMPY_UNDERFLOW(msg) PyErr_SetString(GMPyExc_Underflow, msg)
#define GMPY_ERANGE(msg) PyErr_SetString(GMPyExc_Erange, msg)
#define GMPY_EXPBOUND(msg) PyErr_SetString(GMPyExc_ExpBound, msg)

#define CHECK_UNDERFLOW(msg) \
    if (mpfr_underflow_p() && context->now.trap_underflow) { \
        GMPY_UNDERFLOW(msg); \
        goto done; \
    }

#define MPFR_CHECK_UNDERFLOW(mpfrt, msg) \
    if (mpfr_zero_p(mpfrt->f) && mpfrt->rc) { \
        context->now.underflow = 1; \
        if (context->now.trap_underflow) { \
            GMPY_UNDERFLOW(msg); \
            goto done; \
        } \
    }

#define CHECK_OVERFLOW(msg) \
    if (mpfr_overflow_p() && context->now.trap_overflow) { \
        GMPY_OVERFLOW(msg); \
        goto done; \
    }

#define MPFR_CHECK_OVERFLOW(mpfrt, msg) \
    if (mpfr_inf_p(mpfrt->f)) { \
        context->now.overflow = 1; \
        if (context->now.trap_overflow) { \
            GMPY_OVERFLOW(msg); \
            goto done; \
        } \
    }

#define CHECK_INEXACT(msg) \
    if (mpfr_inexflag_p() && context->now.trap_inexact) { \
        GMPY_INEXACT(msg); \
        goto done; \
    }

#define MPFR_CHECK_INEXACT(mpfrt, msg) \
    if (mpfrt->rc)  { \
        context->now.inexact = 1; \
        if (context->now.trap_inexact) { \
            GMPY_INEXACT(msg); \
            goto done; \
        } \
    }

#define CHECK_INVALID(msg) \
    if (mpfr_nanflag_p() && context->now.trap_invalid) { \
        GMPY_INVALID(msg); \
        goto done; \
    }

#define MPFR_CHECK_INVALID(mpfrt, msg) \
    if (mpfr_nan_p(mpfrt->f)) { \
        context->now.invalid = 1; \
        if (context->now.trap_invalid) { \
            GMPY_INVALID(msg); \
            goto done; \
        } \
    }

#define CHECK_ERANGE(msg) \
    if (mpfr_erangeflag_p() && context->now.trap_erange) { \
        GMPY_ERANGE(msg); \
        goto done; \
    }

#define CHECK_DIVBY0(msg) \
    if (mpfr_divby0_p() && context->now.trap_divzero) { \
        GMPY_DIVZERO(msg); \
        goto done; \
    }

#define MERGE_FLAGS \
    context->now.underflow |= mpfr_underflow_p(); \
    context->now.overflow |= mpfr_overflow_p(); \
    context->now.invalid |= mpfr_nanflag_p(); \
    context->now.inexact |= mpfr_inexflag_p(); \
    context->now.erange |= mpfr_erangeflag_p(); \
    context->now.divzero |= mpfr_divby0_p();

#define CHECK_FLAGS(NAME) \
    CHECK_DIVBY0("'mpfr' division by zero in "NAME); \
    CHECK_INVALID("'mpfr' invalid operation in "NAME); \
    CHECK_UNDERFLOW("'mpfr' underflow in "NAME); \
    CHECK_OVERFLOW("'mpfr' overflow in "NAME); \
    CHECK_INEXACT("'mpfr' inexact result in "NAME); \

#define MPFR_CHECK_FLAGS(mpfrt, NAME) \
    CHECK_DIVBY0(mpfrt, "'mpfr' division by zero in "NAME); \
    CHECK_INVALID(mpfrt, "'mpfr' invalid operation in "NAME); \
    CHECK_UNDERFLOW(mpfrt, "'mpfr' underflow in "NAME); \
    CHECK_OVERFLOW(mpfrt, "'mpfr' overflow in "NAME); \
    CHECK_INEXACT(mpfrt, "'mpfr' inexact result in "NAME); \

#define SUBNORMALIZE(NAME) \
    if (context->now.subnormalize) \
        NAME->rc = mpfr_subnormalize(NAME->f, NAME->rc, context->now.mpfr_round);

#define MPFR_SUBNORMALIZE(mpfrt) \
    if (context->now.subnormalize) \
        mpfrt->rc = mpfr_subnormalize(mpfrt->f, mpfrt->rc, context->now.mpfr_round);

#define MPFR_CLEANUP_SELF(NAME) \
    SUBNORMALIZE(result); \
    MERGE_FLAGS; \
    CHECK_DIVBY0("'mpfr' division by zero in "NAME); \
    CHECK_INVALID("'mpfr' invalid operation in "NAME); \
    CHECK_UNDERFLOW("'mpfr' underflow in "NAME); \
    CHECK_OVERFLOW("'mpfr' overflow in "NAME); \
    CHECK_INEXACT("'mpfr' inexact result in "NAME); \
  done: \
    Py_DECREF(self); \
    if (PyErr_Occurred()) { \
        Py_XDECREF((PyObject*)result); \
        result = NULL; \
    } \
    return (PyObject*)result;

#define MPFR_CLEANUP_SELF_OTHER(NAME) \
    SUBNORMALIZE(result); \
    MERGE_FLAGS; \
    CHECK_DIVBY0("'mpfr' division by zero in "NAME); \
    CHECK_INVALID("'mpfr' invalid operation in "NAME); \
    CHECK_UNDERFLOW("'mpfr' underflow in "NAME); \
    CHECK_OVERFLOW("'mpfr' overflow in "NAME); \
    CHECK_INEXACT("'mpfr' inexact result in "NAME); \
  done: \
    Py_DECREF(self); \
    Py_DECREF(other); \
    if (PyErr_Occurred()) { \
        Py_XDECREF((PyObject*)result); \
        result = NULL; \
    } \
    return (PyObject*)result;

#define MPFR_CLEANUP_RF(NAME) \
    SUBNORMALIZE(rf); \
    MERGE_FLAGS; \
    if (mpfr_divby0_p() && context->now.trap_divzero) { \
        GMPY_DIVZERO("'mpfr' division by zero in " #NAME); \
        Py_DECREF((PyObject*)rf); \
        return NULL; \
    } \
    if (mpfr_underflow_p() && context->now.trap_underflow) { \
        GMPY_UNDERFLOW("'mpfr' underflow in " #NAME); \
        Py_DECREF((PyObject*)rf); \
        return NULL; \
    } \
    if (mpfr_overflow_p() && context->now.trap_overflow) { \
        GMPY_OVERFLOW("'mpfr' overflow in " #NAME); \
        Py_DECREF((PyObject*)rf); \
        return NULL; \
    } \
    if (mpfr_inexflag_p() && context->now.trap_inexact) { \
        GMPY_INEXACT("'mpfr' inexact result in " #NAME); \
        Py_DECREF((PyObject*)rf); \
        return NULL; \
    } \
    return (PyObject*)rf;

static PyTypeObject Pympfr_Type;
#define Pympfr_AS_MPFR(obj) (((PympfrObject *)(obj))->f)
#define Pympfr_Check(v) (((PyObject*)v)->ob_type == &Pympfr_Type)
/* Verify that an object is an mpfr and the exponent is valid */
#define Pympfr_CheckAndExp(v) \
    (Pympfr_Check(v) && \
        (mpfr_zero_p(Pympfr_AS_MPFR(v)) || \
            (mpfr_regular_p(Pympfr_AS_MPFR(v)) && \
                (Pympfr_AS_MPFR(v)->_mpfr_exp >= context->now.emin) && \
                (Pympfr_AS_MPFR(v)->_mpfr_exp <= context->now.emax) \
            ) \
        ) \
    )

/* Forward declarations begin here. */
static PympfrObject *PyStr2Pympfr(PyObject *s, long base, mpfr_prec_t bits);
static PympzObject *Pympfr2Pympz(PyObject *self);
static PyxmpzObject *Pympfr2Pyxmpz(PyObject *self);
static PympqObject *Pympfr2Pympq(PyObject *self);
static PympfrObject *Pympfr_From_Real(PyObject* obj, mpfr_prec_t bits);




