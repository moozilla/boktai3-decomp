#include "scene/motion/scene_steering_actor.h"
#include "player/player.h"
/* SceneSteeringActor_CreateFixedTravelActor: Creates a travel actor at fixed coordinates aimed toward the primary player */
void SceneSteeringActor_CreateTravelActor(const struct ContactPosition *,
                                          const struct ContactPosition *, s32, s32, s32, s32, s32,
                                          s32, s32, s32, s32, s32, s32);
void SceneSteeringActor_CreateFixedTravelActor(void)
{
    struct ContactPosition origin;
    struct ContactPosition difference;
    struct ContactPosition *delta;
    struct Player **location;
    struct ContactPosition *target;
    s32 angle;
    origin.x = 0x980;
    origin.y = 0x2DC;
    origin.z = 0x880;
    location = &gPlayers[0];
    delta = &difference;
    {
        struct Player *player = *location;
        target = (void *)&player->position;
        {
            u32 x = (u16)player->position.x;
            struct ContactPosition *left = &origin;
            x -= (u16)left->x;
            delta->x = x;
        }
        delta->y = target->y - origin.y;
        delta->z = target->z - origin.z;
    }
    angle = Math_Atan2(delta->x, delta->z);
    SceneSteeringActor_CreateTravelActor(&origin, (void *)&(*location)->position, 20, angle, 60, 4,
                                         12, 4, 24, 0, 0, 0, 0);
}
