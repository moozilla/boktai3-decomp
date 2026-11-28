#ifndef GUARD_INCLUDE_OBJECTS_DECK_CANNONS_CALLBACK_SLOT_LAYOUT_H
#define GUARD_INCLUDE_OBJECTS_DECK_CANNONS_CALLBACK_SLOT_LAYOUT_H

#include "global.h"
#include "scene/collision/contact_position.h"
#include "scene/collision/contact_collider.h"

struct CallbackSlot {
    void (*update)(struct CallbackSlot *);
    void (*destroy)(struct CallbackSlot *);
    u8 animation[0x1C]; /* confidence=low */
    u32 flags; /* confidence=low */
    u8 spriteRegistered; /* confidence=low */
    u8 pad29[0x17];
    struct ContactPosition position; /* confidence=low */
    u8 pad48[0x8];
    struct ContactCollider collider;
    struct ContactPosition savedPosition; /* confidence=low */
    void *owner;
    u16 colors[16]; /* confidence=low */
    u16 timer; /* confidence=low */
    u8 phase; /* confidence=low */
    u8 state;
};

typedef char CallbackSlotSizeCheck[sizeof(struct CallbackSlot) == 0xd4 ? 1 : -1];

#endif
