#include "global.h"
#include "scene_vec.h"
#include "proximity_emitter.h"
#define TileTrigger_IsTetherNearby sub_08107298
extern u32 gUnk_030053F4;
u32 TetherActor_IsState(u32);
struct TileTrigger {
    u32 flags;
    u8 unknown4[0x70];
    u8 animation[0x10];
    struct SceneVec position;
    u32 key;
    u8 unknown90[0x28];
    u16 sequence, delay, unknownBC;
    s16 state;
    u16 triggerFlags;
    u8 forced, frame;
};
u8 sub_08107298(struct TileTrigger *p)
{
    struct ContactPosition difference;
    struct ContactPosition *delta;
    struct SceneVec *position;
    struct ContactPosition *target;
    s32 x, z;
    if (gUnk_030053F4 & 0x800)
        return 0;
    if (TetherActor_IsState(6))
        return 0;
    if (TetherActor_IsState(7))
        return 0;
    target = TetherActor_GetPosition();
    if (!target)
        return 0;
    delta = &difference;
    position = &p->position;
    delta->x = target->x - position->x;
    delta->y = target->y - position->y;
    delta->z = target->z - position->z;
    x = delta->x;
    if (x < 0)
        x = -x;
    if (x > 128)
        return 0;
    z = delta->z;
    if (z < 0)
        z = -z;
    if (z > 128)
        return 0;
    return 1;
}
