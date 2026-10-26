#include "global.h"
#include "game_state.h"

struct CP {
    s16 x, y, z, pad;
};
void DirectionalActor_TryBeginState(u8 *, s32);
void ActorController_StartTurn(u8 *, u32);
u32 ActorController_StartModeTwoTurn(u8 *p, u32 v);
u32 ActorController_StartModeThreeTurn(u8 *p, u32 v);
u32 ActorController_StartModeOneTurn(u8 *p, u32 v);
s32 Terrain_ApplyBlocked(void *, void *, struct CP *);
u32 NavMesh_ConsumeProbeEvent(u16 *p);
s32 ActorController_IsNearTarget(u8 *);
u32 sub_0821FB8C(void *, void *, u32, u32);

void ActorController_UpdateTargetMovement(u8 *p, s32 a, s32 b)
{
    struct CP pos;
    u32 r;
    u8 *q;
    if ((p[0xB3E] & 7) == 0) {
        u8 *g = gGameState;
        pos.x = *(u16 *)(g + 0x30);
        pos.y = *(u16 *)(g + 0x32);
        pos.z = *(u16 *)(g + 0x34);
        r = Terrain_ApplyBlocked(p + 0x18, p + 0x48, &pos);
        if (r == 1) {
            p[0xB3D] = r;
        } else if (ActorController_IsNearTarget(p) == 0) {
            p[0xB3D] = 1;
        }
    }
    q = p + 0xB3D;
    if (*q == 0) {
        DirectionalActor_TryBeginState(p, 0);
    } else if (*q == 1) {
        u32 t = sub_0821FB8C(p + 0x18, p + 0x48, p[0xB39], 0x40);
        if (ActorController_IsNearTarget(p) != 0) {
            *q = 0;
            DirectionalActor_TryBeginState(p, 0);
        } else if (p[0xB41] != 0) {
            if (NavMesh_ConsumeProbeEvent((u16 *)(p + 0x18)) != 0) {
                switch (a) {
                    case 0:
                        ActorController_StartTurn(p, t);
                        break;
                    case 1:
                        ActorController_StartModeOneTurn(p, t);
                        break;
                    case 2:
                        ActorController_StartModeTwoTurn(p, t);
                        break;
                    case 3:
                        ActorController_StartModeThreeTurn(p, t);
                        break;
                }
            } else {
                switch (b) {
                    case 1:
                        DirectionalActor_TryBeginState(p, 0x1f);
                        break;
                    case 2:
                        DirectionalActor_TryBeginState(p, 0x2c);
                        break;
                    case 0:
                        switch (a) {
                            case 0:
                                DirectionalActor_TryBeginState(p, 1);
                                break;
                            case 1:
                                DirectionalActor_TryBeginState(p, 0x13);
                                break;
                            case 2:
                                DirectionalActor_TryBeginState(p, 0x16);
                                break;
                            case 3:
                                DirectionalActor_TryBeginState(p, 0x26);
                                break;
                        }
                        break;
                }
            }
        }
    }
}
