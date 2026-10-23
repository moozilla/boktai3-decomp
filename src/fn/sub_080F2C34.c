#include "global.h"

struct PhaseEntry {
    u32 flags;
    u8 pad4[0x28];
};
struct PhaseDrawable {
    u8 pad0[6];
    u16 flags;
    u8 pad8[0x4c];
};
struct PhaseObject {
    u8 pad0[0x8d];
    u8 flags8D;
    u8 pad8E[4];
    u8 mode, step;
    u8 pad94[0x14];
    struct PhaseEntry entries[4];
    u8 pad158[0x70];
    struct PhaseDrawable drawableA[4], drawableB[4];
    u8 pad468[0x48];
    s8 direction;
    u8 pad4B1[7];
    s32 countdown;
    u8 pad4BC[0xb54 - 0x4bc];
    u16 decrement;
    u8 padB56[0xcc8 - 0xb56];
    void (*update)(struct PhaseObject *);
    u8 padCCC[0x10];
    void (*callback)(struct PhaseObject *);
    u8 padCE0[0xd80 - 0xce0];
    u32 frame;
};
void ChildGroup_UpdateTrailingMembers(struct PhaseObject *);
void sub_080F2BE4(struct PhaseObject *);
void PhaseObject_TickCountdown(struct PhaseObject *p)
{
    s32 i;
    if (!(p->frame & 3)) {
        if (p->countdown <= 0) {
            void (*callback)(struct PhaseObject *);
            p->direction = -p->direction;
            {
                u8 one = 1;
                u8 two = 2;
                p->mode = one;
                p->step = two;
                p->flags8D |= one;
            }
            p->update = ChildGroup_UpdateTrailingMembers;
            for (i = 0; i <= 3; i++) {
                p->entries[i].flags |= 1;
                /* set in the loop: loop.c hoists it between the loop's
                   other invariants, where the ROM loads it */
                callback = sub_080F2BE4;
                p->drawableA[i].flags |= 4;
                p->drawableB[i].flags |= 4;
            }
            p->callback = callback;
        } else {
            p->countdown -= p->decrement;
            {
                s32 n = p->countdown;
                if (n < 0)
                    n = 0;
                p->countdown = n;
            }
        }
    }
}
