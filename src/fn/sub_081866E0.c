#include "global.h"
#include "game_state.h"

/* tentative: 13-state axis-aligned chase toward a target */

struct ArmorActor {
    u8 unk0[0x48];
    s16 x;
    s16 unk4a;
    s16 y;
    u8 unk4e[0xB32 - 0x4E];
    u16 flags;
    u8 unkB34[0xB39 - 0xB34];
    u8 facing;
    u8 unkB3A[0xB3D - 0xB3A];
    u8 phase;
};
struct ArmorTarget {
    u8 unk0[0x30];
    s16 x;
    s16 unk32;
    s16 y;
};
s32 DirectionalActor_TryBeginState(struct ArmorActor *, s32);
s32 ActorController_StartModeTwoTurn(struct ArmorActor *, s32);

void Armor_UpdateAxisChase(struct ArmorActor *p, s32 fast)
{
    struct ArmorTarget *g = gGameState;
    s32 dx = g->x - p->x;
    s32 dy = g->y - p->y;
    s32 speed = 0x19;
    if (fast)
        speed = 0x2d;
loop:
    switch (p->phase) {
        case 0:
            if (dx > 0) {
                if (p->facing == 0)
                    p->phase = 3;
                else
                    p->phase = 12;
            } else if (dx < 0) {
                if (p->facing == 0x80)
                    p->phase = 1;
                else
                    p->phase = 11;
            } else if (dy >= 0) {
                if (p->facing == 0x40)
                    p->phase = 7;
                else
                    p->phase = 10;
            } else {
                if (p->facing == 0xc0)
                    p->phase = 5;
                else
                    p->phase = 9;
            }
            goto loop;
        case 1:
            if (DirectionalActor_TryBeginState(p, speed))
                p->phase = 2;
            break;
        case 2:
            if (dx > 0x200 || (p->flags & 0x10)) {
                if (dy < 0)
                    p->phase = 9;
                else
                    p->phase = 10;
            }
            break;
        case 3:
            if (DirectionalActor_TryBeginState(p, speed))
                p->phase = 4;
            break;
        case 4:
            if (dx < -0x200 || (p->flags & 0x10)) {
                if (dy < 0)
                    p->phase = 9;
                else
                    p->phase = 10;
            }
            break;
        case 5:
            if (DirectionalActor_TryBeginState(p, speed))
                p->phase = 6;
            break;
        case 6:
            if (dy > 0x200 || (p->flags & 0x10)) {
                if (dx < 0)
                    p->phase = 11;
                else
                    p->phase = 12;
            }
            break;
        case 7:
            if (DirectionalActor_TryBeginState(p, speed))
                p->phase = 8;
            break;
        case 8:
            if (dy < -0x200 || (p->flags & 0x10)) {
                if (dx < 0)
                    p->phase = 11;
                else
                    p->phase = 12;
            }
            break;
        case 9:
            if (ActorController_StartModeTwoTurn(p, 0xc0))
                p->phase = 5;
            break;
        case 10:
            if (ActorController_StartModeTwoTurn(p, 0x40))
                p->phase = 7;
            break;
        case 11:
            if (ActorController_StartModeTwoTurn(p, 0x80))
                p->phase = 1;
            break;
        case 12:
            if (ActorController_StartModeTwoTurn(p, 0))
                p->phase = 3;
            break;
    }
}
