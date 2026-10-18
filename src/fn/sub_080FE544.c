#include "global.h"
#include "scene_actor_reset.h"
struct SceneActorReactionCountContext {
    u8 unknown0[0xE4C];
    u32 flagsE4C;
    u8 unknownE50[0x60];
    u8 reactionCount;
    u8 unknownEB1[0x23];
    u16 fieldED4;
};
static inline u8 TestFlag(u32 mask, u32 *flags)
{
    if (*flags & mask)
        return 1;
    return 0;
}
static inline u32 MaskFlags(u16 *p, u32 mask)
{
    return *p & mask;
}
s32 SceneActorControl_AdvanceReactionCount(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorReactionCountContext *context = actor->context;
    u16 *flags;
    u32 mask;
    if (actor->reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x13E);
    flags = &actor->flags2D4;
    mask = ~0x20;
    *flags = mask & *flags;
    if (*flags & 0x8000)
        context->fieldED4 = 8;
    if (context->reactionCount <= 2)
        ++context->reactionCount;
    if (!MaskFlags(flags, 0x8100)) {
        u32 blocked = TestFlag(4, &context->flagsE4C);
        if (!blocked && context->reactionCount > 2) {
            u32 phase = 2;
            u32 behavior = 6;
            actor->state2A8 = blocked;
            actor->phase2A9 = phase;
            actor->behavior2AA = behavior;
            actor->counter2AC = blocked;
            actor->field2B1 = 1;
            context->reactionCount = blocked;
            return 1;
        }
    }
    return 1;
}
