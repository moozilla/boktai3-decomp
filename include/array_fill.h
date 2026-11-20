#ifndef GUARD_ARRAY_FILL_H
#define GUARD_ARRAY_FILL_H

#include "gba/types.h"

/* Fill a nonempty range of array elements, including both endpoints.
 * The first element must be a side-effect-free lvalue. Walk GBA addresses
 * so the final decrement does not form a pointer before the array. */
#define Array_FillDescending(first, last, value)                                                   \
    do {                                                                                           \
        __typeof__(last) fillValue = (value);                                                      \
        s32 cursor = (s32) & (last);                                                               \
        do {                                                                                       \
            *(__typeof__(&(last)))cursor = fillValue;                                              \
            cursor -= (s32)sizeof(last);                                                           \
        } while (cursor >= (s32) & (first));                                                       \
    } while (0)

#endif
