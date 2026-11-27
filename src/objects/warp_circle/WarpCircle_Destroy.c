#include "objects/warp_circle/warp_circle.h"
void Sprite_Remove(void *);
void Effect_Remove(struct WarpCircleParticle *);
void SceneMessages_UnregisterReceiver(void *);
s32 WarpCircle_Destroy(struct WarpCircle *actor)
{
    struct WarpCircleSprite *segment;
    struct WarpCircleParticle *particle;
    s32 i;
    void *trigger;
    Sprite_Remove(&actor->sprite);
    Sprite_Remove(&actor->shadow);
    segment = actor->segments;
    i = 2;
    do {
        Sprite_Remove(segment);
        segment++;
        i--;
    } while (i >= 0);
    trigger = actor->trigger;
    particle = actor->particles;
    i = 7;
    do {
        Effect_Remove(particle);
        particle++;
        i--;
    } while (i >= 0);
    SceneMessages_UnregisterReceiver(trigger);
    if (gWarpCircle)
        gWarpCircle = 0;
    return 0;
}
