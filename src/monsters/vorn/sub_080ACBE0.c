#include "global.h"
#include "scene/actor/scene_actor_reset.h"
#include "player/player.h"
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
static inline s32 TruncateShift(s32 value, u32 shift)
{
    if (value >= 0)
        return value >> shift;
    return -(-value >> shift);
}
void SceneActor_AimArcAtPlayer(struct ChaseActor *actor)
{
    struct ContactPosition delta;
    struct SceneActorRegistrySequenceDrawable *drawable = actor->base.drawable;
    struct Player **slots = gPlayers;
    struct ContactPosition *out = &delta;
    const struct ContactPosition *own = &drawable->position;
    const struct SceneVec *player = &slots[0]->position;
    s32 angle, time;
    delta.x = player->x - own->x;
    out->y = player->y - own->y;
    out->z = player->z - own->z;
    angle = Math_Atan2(delta.x, out->z);
    actor->base.secondaryMotion.x = TruncateShift(gUnk_086149C4[(angle + 64) & 255] * 32, 12);
    actor->base.secondaryMotion.z = TruncateShift(gUnk_086149C4[angle & 255] * 32, 12);
    if (actor->base.secondaryMotion.x != 0)
        time = Div(delta.x, actor->base.secondaryMotion.x);
    else if (actor->base.secondaryMotion.z != 0)
        time = Div(delta.z, actor->base.secondaryMotion.z);
    else
        time = 0;
    actor->base.secondaryMotion.y = TruncateShift(time, 1);
}
