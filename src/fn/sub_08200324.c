#define PlayerClone_OnBodyColliderContact sub_08200324
#include "global.h"

struct CloneContact {
    u8 unknown00[0x38];
    u32 flags;
    u16 strength;
    u8 unknown3e[2];
    u16 effect;
    u8 unknown42[2];
    u16 duration;
};
struct PlayerClone {
    u8 unknown000[0xc4];
    u16 contactTimer;
    u8 unknown0c6[0x26a];
    u16 reactionTimer;
    u8 unknown332[2];
    u16 hitPending;
    u16 hitStrength;
    u16 hitEffect;
    u16 hitDuration;
};
void Sound_PlaySong(u32);

void sub_08200324(struct CloneContact *contact, struct CloneContact *other, struct PlayerClone *s)
{
    u16 zero = other->duration;
    if (zero == 0) {
        if (contact->flags & 0x40) {
            Sound_PlaySong(0x211);
            s->hitPending = 1;
            s->hitStrength = zero;
            s->hitEffect = zero;
            s->hitDuration = zero;
        } else {
            s->hitPending = 1;
            s->hitStrength = contact->strength >> 2;
            s->hitEffect = contact->effect;
            s->hitDuration = contact->duration;
            Sound_PlaySong(0x4f4);
        }
        s->reactionTimer = 16;
        s->contactTimer = 16;
    }
}
