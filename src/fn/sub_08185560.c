#include "global.h"
#include "game_state.h"
struct P {
    u32 x : 16, y : 16, z : 16, w : 16;
};
s32 Math_Atan2(s32, s32);
void ActorController_StartTurn(void *);
s32 Armor_TryStartAction(void *, s32);
void Armor_UpdateFacingAction9(u8 *p)
{
    u8 *state = p + 0xb3d;
    switch (*state) {
        case 0: {
            struct P v;
            u8 *actor = gGameState;
            s32 angle;
            v.x = *(u16 *)(actor + 0x30);
            v.y = *(u16 *)(actor + 0x32);
            v.z = *(u16 *)(actor + 0x34);
            angle = Math_Atan2((s16)v.x - *(s16 *)(p + 0x48), (s16)v.z - *(s16 *)(p + 0x4c));
            angle = (angle + 0x20) & 0xc0;
            if (p[0xb39] != angle)
                ActorController_StartTurn(p);
            else if (Armor_TryStartAction(p, 9))
                *state = 1;
            break;
        }
        case 1:
            if (p[0xb40] == 0xff)
                *state = 0xff;
            break;
    }
}
