#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"
#include "ram.h"

#define gSceneActorSineTable gUnk_086149C4
extern const s16 gUnk_086149C4[];
s32 Div(s32, s32);
struct ActorHeightOscillation {
    u8 unknown0[0x5B6];
    u8 angle;
};
static inline void EaseHeight(s32 target, s16 *height)
{
    *height += Div(target - *height, 10);
}
static inline s32 ScaleWave(s32 wave)
{
    if (wave >= 0)
        return wave >> 12;
    return -(-wave >> 12);
}
void SceneActor_EasePlayerHeight(struct SceneActorRegistryResetView *actor)
{
    struct ActorHeightOscillation *state = actor->context;
    u32 active = gPlayerPassageState;
    s32 special = 0;
    s32 offset;
    s32 target;
    if (active && gPlayerInteractionMode == 5)
        special = 1;
    if (special || gPlayers[0]->actionMode == 4)
        offset = 100;
    else
        offset = 220;
    if (actor->field2B5 == 0x23) {
        target = gPlayers[0]->position.y + offset;
        EaseHeight(target, &actor->body.position.y);
    } else {
        s32 wave = gUnk_086149C4[state->angle] << 5;
        target = gPlayers[0]->position.y + offset + ScaleWave(wave);
        EaseHeight(target, &actor->body.position.y);
        state->angle += 2;
    }
}
