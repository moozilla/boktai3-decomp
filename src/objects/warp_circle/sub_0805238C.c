#include "objects/warp_circle/warp_circle.h"
#include "objects/coffin/tether_actor.h"
struct WarpCircleTetherView {
    u8 unknown0[0x1A], mode, unknown1B[0x23A], phase;
};
struct WarpCirclePosition *TetherActor_GetPosition(void);
s32 WarpCircle_IsTetherNearby(struct WarpCircle *o)
{
    struct WarpCirclePosition *v = TetherActor_GetPosition();
    if (v != 0 && (*(struct WarpCircleTetherView **)&gTetherActor)->mode == 4 &&
        (*(struct WarpCircleTetherView **)&gTetherActor)->phase <= 4) {
        s32 d = v->y - o->world.y;
        if (d < 0)
            d = -d;
        if (d <= 0xff) {
            s32 dx = v->x - o->world.x;
            s32 dz = v->z - o->world.z;
            if (dx <= 0x17c && dz <= 0x17c) {
                if (dx * dx + dz * dz <= 0x23410)
                    return 1;
            }
        }
    }
    return 0;
}
