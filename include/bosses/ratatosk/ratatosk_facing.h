#ifndef GUARD_RATATOSK_FACING_H
#define GUARD_RATATOSK_FACING_H

#include "engine/shared/global.h"

#define RATATOSK_SET_FACING(actor_, heading_)                                                      \
    do {                                                                                           \
        __typeof__(actor_) facingActor = (actor_);                                                 \
        s32 shifted;                                                                               \
        u32 value;                                                                                 \
        facingActor->body.direction = (heading_);                                                  \
        shifted = (facingActor->body.direction + 0x60) >> 6;                                       \
        value = 3;                                                                                 \
        value &= shifted;                                                                          \
        facingActor->facing = value;                                                               \
        shifted = (facingActor->body.direction + 0x70) >> 5;                                       \
        value = 7;                                                                                 \
        value &= shifted;                                                                          \
        facingActor->facing8 = value;                                                              \
    } while (0)

#endif
