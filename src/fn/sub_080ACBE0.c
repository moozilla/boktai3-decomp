#define gSineTable gUnk_086149C4
#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"
extern const s16 gSineTable[];
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
static inline s32 TruncateShift(s32 value, u32 shift)
{
    if (value >= 0)
        return value >> shift;
    return -(-value >> shift);
}
void SceneActor_AimArcAtPlayer(struct SceneActorRegistryResetView *actor)
{
    struct ContactPosition delta;
    struct SceneActorRegistrySequenceDrawable *drawable = actor->drawable;
    struct Player **slots = gPlayers;
    struct ContactPosition *out = &delta;
    const struct ContactPosition *own = &drawable->position;
    const struct SceneVec *player = &slots[0]->position;
    s32 angle, time;
    delta.x = player->x - own->x;
    out->y = player->y - own->y;
    out->z = player->z - own->z;
    angle = Math_Atan2(delta.x, out->z);
    actor->secondaryMotion.x = TruncateShift(gSineTable[(angle + 64) & 255] * 32, 12);
    actor->secondaryMotion.z = TruncateShift(gSineTable[angle & 255] * 32, 12);
    if (actor->secondaryMotion.x != 0)
        time = Div(delta.x, actor->secondaryMotion.x);
    else if (actor->secondaryMotion.z != 0)
        time = Div(delta.z, actor->secondaryMotion.z);
    else
        time = 0;
    actor->secondaryMotion.y = TruncateShift(time, 1);
}
