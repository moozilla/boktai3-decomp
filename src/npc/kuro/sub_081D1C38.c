#include "global.h"
#include "engine/engine.h"
#include "engine/math/math_shift.h"
#include "scene/motion/scene_facing.h"
#include "npc/manager/scene_actor_lifecycle.h"

extern const s16 gUnk_086149C4[];

int Math_Atan2(int, int);
void Kuro_PlayTaggedAnimation(struct SceneActor *, int, int, int, int);

// confidence=low: advances toward the scripted position using the current movement angle
// (animation 3; sub_081D1A8C is the same walk with animation 1 at speed 5).
void sub_081D1C38(void *unused, struct SceneActor *actor)
{
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        Kuro_PlayTaggedAnimation(actor, 3, 1, 0, 4);
    }
    {
        struct ContactPosition *pos = &actor->body.position;
        struct ContactPosition *target = &actor->target;
        int dx = target->x - pos->x;
        int dz = target->z - pos->z;
        int distance = dx * dx + dz * dz;
        struct ContactPosition *from = pos;
        struct ContactPosition *to = target;
        if (distance <= 100) {
            actor->body.position = actor->target;
            actor->finished = 1;
        } else {
            u8 *facing = &actor->body.direction;
            struct ContactPosition *velocity = &actor->body.motion;
            int angle = *facing;
            int speed = 8;
            s32 product = speed * gUnk_086149C4[(angle + 64) & 255];
            u8 *savedFacing = facing;
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
            Kuro_PlayTaggedAnimation(actor, 3, 1, 0, 5);
        }
    }
}
