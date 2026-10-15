#include "scene_steering_actor.h"
#define SceneSteeringActor_TurnTowardTarget sub_0806BCD8
void SceneSteeringActor_TurnTowardTarget(struct SceneSteeringActor *p)
{
    if (p->steeringDelay) {
        p->steeringDelay--;
        return;
    }
    if (p->steeringTimer) {
        struct ContactPosition differenceVector;
        struct ContactPosition *delta = &differenceVector;
        s32 angle, difference, magnitude;
        {
            const struct ContactPosition *right = &p->position;
            const struct ContactPosition *left = &p->target;
            differenceVector.x = p->target.x - p->position.x;
            delta->y = left->y - right->y;
            delta->z = left->z - right->z;
        }
        angle = Math_Atan2(differenceVector.x, delta->z);
        difference = angle - p->direction;
        if (difference > 128)
            difference -= 256;
        else if (difference < -128)
            difference += 256;
        magnitude = difference;
        if (difference < 0)
            magnitude = -difference;
        if (magnitude > 18) {
            if (difference > 0)
                p->direction += p->turnStep;
            else
                p->direction -= p->turnStep;
            p->steeringTimer--;
        } else
            p->steeringTimer = 0;
    }
}
