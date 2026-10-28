#include "stacked_actor.h"
#include "tether_actor.h"
struct StackedActorTetherView {
    u8 unknown0[0x1A], mode, unknown1B[0x23A], phase;
};
struct StackedActorPosition *TetherActor_GetPosition(void);
s32 StackedActor_IsTetherNearby(struct StackedActor *o)
{
    struct StackedActorPosition *v = TetherActor_GetPosition();
    if (v != 0 && (*(struct StackedActorTetherView **)&gTetherActor)->mode == 4 &&
        (*(struct StackedActorTetherView **)&gTetherActor)->phase <= 4) {
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
