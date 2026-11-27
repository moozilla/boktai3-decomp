#ifndef GUARD_INCLUDE_OBJECTS_DECK_CANNONS_CALLBACK_SLOT_LAYOUT_H
#define GUARD_INCLUDE_OBJECTS_DECK_CANNONS_CALLBACK_SLOT_LAYOUT_H

#include "global.h"
#include "scene/collision/contact_position.h"

struct CallbackSlot {
    u32 update;
    u32 destroy;
    u8 animation[0x1C]; /* confidence=low */
    u32 flags; /* confidence=low */
    u8 pad28[0x18];
    struct ContactPosition position; /* confidence=low */
    u8 pad48[0x8];
    u8 collider[0x54];
    struct ContactPosition savedPosition; /* confidence=low */
    void *owner;
    u16 colors[16]; /* confidence=low */
    u16 timer; /* confidence=low */
    u8 phase; /* confidence=low */
    u8 state;
};

typedef char CallbackSlotSizeCheck[sizeof(struct CallbackSlot) == 0xd4 ? 1 : -1];

#endif
