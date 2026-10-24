#include "global.h"
#include "scene_vec.h"

// confidence=low: the cannon turret's wait state, shaped like ProximitySequence_TestPlayerDistance:
// once the player is closer than the trigger distance, switch to the aimed volley for 180 frames.
struct TurretProximity {
    u8 unknown00[0x40];
    struct SceneVec position;
    u8 unknown48[0x108 - 0x48];
    s32 (*update)(struct TurretProximity *);
    u8 unknown10C[0x11c - 0x10c];
    s32 triggerDistanceSquared;
    u8 unknown120[6];
    u16 timer;
};
struct SceneVec *Player_GetEffectivePosition(s32);
s32 CannonTurret_FireAimedVolley(struct TurretProximity *);

s32 sub_0820843C(struct TurretProximity *s)
{
    struct SceneVec *player = Player_GetEffectivePosition(0);
    struct SceneVec *position = &s->position;
    s32 dx = player->x - s->position.x;
    s32 dz = player->z - position->z;
    if (dx * dx + dz * dz < s->triggerDistanceSquared) {
        s->update = CannonTurret_FireAimedVolley;
        s->timer = 180;
    }
    return 0;
}
