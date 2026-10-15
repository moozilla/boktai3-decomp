#include "scene_steering_actor.h"
void SceneSteeringActor_RequestDeparture(u32 a, u32 b, struct SceneSteeringActor *p)
{
    p->state = 5;
    p->enter = 1;
}
