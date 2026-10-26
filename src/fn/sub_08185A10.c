#include "global.h"
#include "game_state.h"
struct Armor {
    u8 pad0[0x18];
    u8 navigation[0x30];
    s16 position[3];
    u8 pad4E[0xb39 - 0x4e];
    u8 direction;
    u8 padB3A[3];
    u8 state, timer;
};
struct ArmorPackedPosition {
    u32 x : 16, y : 16, z : 16, unused : 16;
};
s32 Terrain_ApplyBlocked(void *, void *, struct ArmorPackedPosition *);
u32 sub_0821FB8C(void *, void *, u32, u32);
s32 DirectionalActor_TryBeginState(struct Armor *, s32);
void Armor_UpdateTargetFacing(struct Armor *p)
{
    struct ArmorPackedPosition pos;
    if ((p->timer & 3) == 0) {
        u8 *target = gGameState;
        pos.x = *(u16 *)(target + 0x30);
        pos.y = *(u16 *)(target + 0x32);
        pos.z = *(u16 *)(target + 0x34);
        Terrain_ApplyBlocked(p->navigation, p->position, &pos);
    }
    sub_0821FB8C(p->navigation, p->position, p->direction, 0x40);
    if (p->state == 0) {
        if (DirectionalActor_TryBeginState(p, 0x1c))
            p->state = 255;
    }
}
