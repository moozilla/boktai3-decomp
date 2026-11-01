#ifndef GUARD_NAMED_OBJECT_FLAGS_H
#define GUARD_NAMED_OBJECT_FLAGS_H
#include "global.h"

/* The flags halfword of a named scene object (+0xB8). Packed, so that each
 * update stores through a field insertion: the original's set and clear helpers
 * leave that insertion's zero (`movs rN, #0` between the ldrh and the orrs) in
 * the code (scope README facts 34 and 36). */
struct NamedObjectFlags {
    u16 value;
} __attribute__((packed, aligned(2)));

static inline void NamedObjectFlags_Set(struct NamedObjectFlags *field, u16 bits)
{
    field->value = bits | field->value;
}

static inline void NamedObjectFlags_Clear(struct NamedObjectFlags *field, s32 bits)
{
    field->value &= ~bits;
}
#endif
