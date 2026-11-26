#include "monsters/garmr_statue.h"
#include "engine/math/scene_vec.h"

// confidence=low: the cannon turret's wait state, shaped like ProximitySequence_TestPlayerDistance:
// once the player is closer than the trigger distance, switch to the aimed volley for 180 frames.
struct SceneVec *Player_GetEffectivePosition(s32);
s32 GarmrStatue_FireAimedVolley(struct GarmrStatue *);

s32 sub_0820843C(struct GarmrStatue *s)
{
    struct SceneVec *player = Player_GetEffectivePosition(0);
    struct SceneVec *position = &s->sprite.coordinates.position;
    s32 dx = player->x - s->sprite.coordinates.position.x;
    s32 dz = player->z - position->z;
    if (dx * dx + dz * dz < s->triggerDistanceSquared) {
        s->update = (void (*)(void))GarmrStatue_FireAimedVolley;
        s->fireTimer = 180;
    }
    return 0;
}
