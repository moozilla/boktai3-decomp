#ifndef GUARD_LIBGCC_NUMBERS_H
#define GUARD_LIBGCC_NUMBERS_H

/* RAM number layouts from FSF fp-bit-base.c; license/exception in src/lib/libgcc_*.c. */
typedef enum { CLASS_SNAN, CLASS_QNAN, CLASS_ZERO, CLASS_NUMBER, CLASS_INFINITY } fp_class_type;

struct SoftDoubleNumber {
    fp_class_type class;
    unsigned int sign;
    int normal_exp;
    /* fp-bit-base.c exposes the fraction as a 64-bit word and two 32-bit halves. */
    union {
        unsigned long long ll;
        unsigned int l[2];
    } fraction;
};

struct SoftSingleNumber {
    fp_class_type class;
    unsigned int sign;
    int normal_exp;
    /* fp-bit-base.c exposes the fraction as a 32-bit word and two 16-bit halves. */
    union {
        unsigned int ll;
        unsigned short l[2];
    } fraction;
};

/* 0x030035C8 */
extern struct SoftDoubleNumber gSoftDoubleNaN;

/* 0x030035E0 */
extern struct SoftSingleNumber gSoftSingleNaN;

#endif
