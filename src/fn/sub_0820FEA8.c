#include "global.h"
/* Names and field interpretations are tentative. */

struct HitSource {
    u8 pad0[0xA];
    u16 damageType;
    u8 padC[0x2C];
    u32 flags;
    u8 pad3C[8];
    u16 reaction;
};
struct HitResult {
    u8 pad0[0x3E];
    u16 damage;
    u8 pad40[3];
    u8 state;
    u16 blocked;
};
struct HitRecipient {
    u8 pad0[0x6C];
    u8 display[0xC];
    u8 position[8];
    u8 displayPosition[0x30];
    u16 reaction;
    u8 padB2[0x7A];
    u16 timer;
    u8 pad12E[2];
    s16 damage;
    u8 pad132[0x16];
    u32 damageType;
};
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void Sound_PlaySong(u32);
void PlayerStats_AddContactHundredths(void *, void *, u32);
void sub_0821FF8C(void *);

void Combat_ApplyHitReaction(struct HitSource *source, struct HitResult *hit,
                             struct HitRecipient *target)
{
    sub_0821FF8C(source);
    if (hit->blocked) {
        hit->damage = 0;
        hit->state = 0;
    } else {
        target->damage = hit->damage;
        target->damageType = source->damageType;
        target->reaction = source->reaction;
        if (source->flags & 0x40) {
            target->reaction = 0x26;
            target->timer = 8;
            Sound_PlaySong(0x211);
            DamageNumberDisplay_Show(target->display, target->damage, 0, target->position,
                                     target->displayPosition, 1);
            if (source->flags & 0x40)
                return;
        }
        PlayerStats_AddContactHundredths(source, hit, 0);
    }
}
