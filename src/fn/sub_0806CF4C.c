#include "scene_steering_actor.h"
#include "player.h"
/* SceneSteeringActor_CreateFixedOriginTravelActor: Creates a travel actor at fixed coordinates aimed toward the primary player */
#define SceneSteeringActor_CreateFixedOriginTravelActor sub_0806CF4C
struct SceneSteeringPlayerView {
    u8 unknown0[0x30];
    struct ContactPosition position;
};
void SceneSteeringActor_CreateTravelActor(const struct ContactPosition *,
                                          const struct ContactPosition *, s32, s32, s32, s32, s32,
                                          s32, s32, s32, s32, s32, s32);
#define SceneSteeringActor_CreateFixedTravelActor sub_0806CF4C
void SceneSteeringActor_CreateFixedTravelActor(void)
{
    union {
        struct ContactPackedVector packed;
        struct ContactPosition position;
    } origin;
    struct ContactPosition difference;
    struct ContactPosition *delta;
    struct SceneSteeringPlayerView **location;
    struct ContactPosition *target;
    s32 angle;
    u32 zero;
    origin.packed.x = 0x980;
    origin.packed.y = 0x2DC;
    origin.packed.z = 0x880;
    location = &gPlayers[0];
    delta = &difference;
    {
        struct SceneSteeringPlayerView *player = *location;
        target = &player->position;
        {
            u32 x = (u16)player->position.x;
            struct ContactPosition *left = &origin.position;
            x -= (u16)left->x;
            zero = 0;
            delta->x = x;
        }
        delta->y = target->y - origin.position.y;
        delta->z = target->z - origin.position.z;
    }
    angle = Math_Atan2(delta->x, delta->z);
    SceneSteeringActor_CreateTravelActor(&origin.position, &(*location)->position, 20, angle, 60, 4,
                                         12, 4, 24, zero, zero, zero, zero);
}
