#include "global.h"
#include "engine/engine.h"
#include "engine/math/math_shift.h"
#include "scene/motion/scene_facing.h"
#include "npc/manager/scene_actor_lifecycle.h"

extern const s16 gUnk_086149C4[];

int Math_Atan2(int, int);

// confidence=low: Actor_UpdateWalkToPosition without the entry animation, callback test or
// footsteps; it moves by the new velocity itself.
void sub_081CBB1C(void *unused, struct SceneActor *actor)
{
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
    }
    {
        struct ContactPosition *pos = &actor->body.position;
        struct ContactPosition *target = &actor->target;
        int dx = target->x - pos->x;
        int dz = target->z - pos->z;
        u32 distance = dx * dx + dz * dz;
        u32 limit = actor->distanceSquared;
        struct ContactPosition *from = pos;
        struct ContactPosition *to = target;
        if (distance <= limit) {
            actor->body.position = actor->target;
            actor->finished = 1;
        } else {
            u8 *facing = &actor->body.direction;
            struct ContactPosition *velocity = &actor->body.motion;
            int angle = *facing;
            int speed = actor->distance;
            s32 product = speed * gUnk_086149C4[(angle + 64) & 255];
            u8 *savedFacing = facing;
            struct ContactPosition *savedVelocity = velocity;
            velocity->x = Math_ShiftTowardZero(product, 12);
            velocity->y = 0;
            velocity->z = Math_ShiftTowardZero(speed * gUnk_086149C4[angle], 12);
            actor->direction = Math_Atan2(to->x - from->x, to->z - from->z);
            {
                int value = actor->direction;
                *savedFacing = value;
                actor->facing8 = SceneFacing_EightWay(value);
                actor->facing4 = SceneFacing_FourWay(value);
            }
            from->x += savedVelocity->x;
            actor->body.position.z += actor->body.motion.z;
        }
    }
}
