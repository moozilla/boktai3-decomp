#include "global.h"
#include "player.h"

u32 sub_08220204(struct Player *, void *);
s32 sub_08249244(void *, struct Player *, void *, void *);
s32 PlayerLink_IsActionModeAllowed(void *);

struct ContactCostTarget {
    u8 unk0[6];
    u16 flags6;
    u8 unk8[2];
    u16 flagsA;
    u8 unkC[0x32];
    u16 cost;
    u8 unk40[3];
    u8 flags43;
};

void Player_ApplyContactCosts(struct Player *p, struct ContactCostTarget *t)
{
    struct Player *m;
    s32 i;
    s32 mask = 0x1F;
    if ((t->flagsA & mask) != 0) {
        m = *(struct Player **)((u8 *)p + 0x50);
        sub_08220204(p, t);
        i = 0;
        do {
            void *cb = m->contactCostCallbacks[i];
            if (cb)
                t->cost += sub_08249244(m, p, t, cb);
            i++;
        } while (i <= 2);
        if (m->actionMode == 1 && m->statModifierContext.savedActionIndex == 1 &&
            PlayerLink_IsActionModeAllowed(m) != 0) {
            if (!(t->flags43 & 8)) {
                t->cost >>= 2;
                if (t->flags6 & 0x100)
                    t->flags43 |= 8;
            }
        }
    }
}
