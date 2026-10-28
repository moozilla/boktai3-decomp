#define gHoverAnimationData gUnk_0824E318
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#include "player.h"
extern const u8 gUnk_0824E318[];
void SceneActorReaction_ResetResponseState(void *, const void *, u32);
struct HoverControlContext {
    u8 unknown0[0xE4C];
    u32 savedFlags;
    u8 unknownE50[0x58];
    u16 timer;
    u8 angle, enabled;
    u16 counter;
    u8 unknownEAE[0x56];
    struct ContactPosition vectorF04, vectorF0C, vectorF14, savedPosition;
};
static inline void Fill(void *dest, u32 *zero)
{
    *zero = 0;
    CpuSet(zero, dest, 0x05000002);
}
void SceneActorControl_InitializeHoverState(struct SceneActorRegistryResetView *actor)
{
    struct HoverControlContext *context = actor->context;
    u32 zero;
    context->timer = 0;
    context->angle = 0;
    context->enabled = 1;
    context->counter = 0;
    context->savedFlags = actor->flags130;
    context->savedPosition = *(struct ContactPosition *)((*(u8 **)gPlayers) + 0x30);
    Fill(&context->vectorF04, &zero);
    Fill(&context->vectorF0C, &zero);
    Fill(&context->vectorF14, &zero);
    Memory_Clear(actor->state5D4, 8);
    SceneActorReaction_ResetResponseState(&actor->word460, gHoverAnimationData, 0xD4);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
}
