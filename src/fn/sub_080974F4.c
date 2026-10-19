#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"

#ifdef NONMATCHING
// fuzzy: 96.1%
// foundry draft (worker best 96.1%); what differs is mostly register allocation and instruction order
#define SceneActor_EasePlayerHeight sub_080974F4
#define gSceneActorSineTable        gUnk_086149C4
extern u16 gUnk_02000534, gUnk_02000500;
extern const s16 gUnk_086149C4[];
s32 Div(s32, s32);
struct ActorHeightOscillation {
    u8 unknown0[0x5B6];
    u8 angle;
};
static inline void EaseHeight(s16 *height, s32 target)
{
    *height += Div(target - *height, 10);
}
static inline s32 ScaleWave(s32 wave)
{
    if (wave >= 0)
        return wave >> 12;
    return -(-wave >> 12);
}
void sub_080974F4(struct SceneActorRegistryResetView *actor)
{
    struct ActorHeightOscillation *state = actor->context;
    u32 active = gUnk_02000534;
    s32 special = 0;
    s32 offset;
    if (active && gUnk_02000500 == 5)
        special = 1;
    if (special || gUnk_02000580[0]->actionMode == 4)
        offset = 100;
    else
        offset = 220;
    if (actor->field2B5 == 0x23) {
        s32 target = gUnk_02000580[0]->position.y;
        target += offset;
        EaseHeight(&actor->position.y, target);
    } else {
        s32 wave = gUnk_086149C4[state->angle] << 5;
        s32 delta;
        delta = ScaleWave(wave);
        delta = delta + (gUnk_02000580[0]->position.y + offset);
        EaseHeight(&actor->position.y, delta);
        state->angle += 2;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080974F4);
#endif
