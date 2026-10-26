#include "global.h"
#include "game_state.h"
struct S {
    u8 f0[0x48];
    s16 a48;
    s16 a4a;
    s16 a4c;
    u8 g0[0xB39 - 0x4E];
    u8 b39;
    u8 b3a;
    u8 f1;
    u8 b3c;
    u8 b3d;
    u8 b3e;
    u8 b3f;
    u8 b40;
    u8 b41;
    u8 b42;
    u8 b43;
    u8 f2;
    u8 b45;
    u8 g1[0x1124 - 0xB46];
    s16 x;
    s16 y;
    s16 z;
};
struct G {
    u8 f[0x30];
    s16 x;
    s16 pad;
    s16 y;
};
s32 Math_Atan2(s32, s32);
void ActorController_StartTurn(struct S *, s32);
s32 Armor_TryStartAction(struct S *, s32);
void Armor_UpdateFacingAction6(struct S *p)
{
    u8 *q = &p->b3d;
    u8 r;
    switch (*q) {
        case 0: {
            struct G *g = gGameState;
            s32 a = Math_Atan2(g->x - p->a48, g->y - p->a4c);
            a = (a + 0x20) & 0xc0;
            if (p->b39 != a)
                ActorController_StartTurn(p, a);
            else if (Armor_TryStartAction(p, 6)) {
                r = 1;
                goto st;
            }
            break;
        }
        case 1:
            r = p->b40;
            if (r == 0xff) {
            st:
                *q = r;
            }
            break;
    }
}
