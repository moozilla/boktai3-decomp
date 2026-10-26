#include "global.h"
#include "game_state.h"

/* Tentative Armor-family sequence: face target, action 25, delay, action 43. */
struct ArmorActor {
    u8 unk0[0x48];
    s16 x;
    s16 unk4a;
    s16 y;
    u8 unk4e[0xB39 - 0x4E];
    u8 facing;
    u8 unkB3A;
    u8 direction;
    u8 unkB3C;
    u8 phase;
    u8 timer;
    u8 unkB3F;
    u8 actionStatus;
};
struct ArmorTarget {
    u8 unk0[0x30];
    s16 x;
    s16 unk32;
    s16 y;
};
s32 Math_Atan2(s32, s32);
u32 ActorController_StartAlternateTurn(struct ArmorActor *, u32);
s32 DirectionalActor_TryBeginState(struct ArmorActor *, s32);

void Armor_UpdateFacingActionSequence(struct ArmorActor *p)
{
    u8 *q = &p->phase;
    u8 r;
    switch (*q) {
        case 0: {
            struct ArmorTarget *g = gGameState;
            s32 a = Math_Atan2(g->x - p->x, g->y - p->y);
            a = (a + 0x20) & 0xc0;
            if (p->facing != a) {
                if (ActorController_StartAlternateTurn(p, a)) {
                    r = 1;
                    goto st;
                }
                break;
            }
            *q = 1;
        }
        case 1:
            if (DirectionalActor_TryBeginState(p, 0x19))
                p->phase = 2;
            break;
        case 2:
            if (p->timer > 9 && DirectionalActor_TryBeginState(p, 0x2b)) {
                r = 3;
                goto st;
            }
            break;
        case 3:
            r = p->actionStatus;
            if (r == 0xff) {
            st:
                *q = r;
            }
            break;
    }
}
