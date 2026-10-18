#define SceneActor_UpdateDelayedAnimation sub_080ACEE8
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
struct DelayedAnimationContext {
    u8 unknown0[0x694];
    u16 flags;
    u8 unknown696, delay;
};
static inline u8 TakeEntryFlag(struct SceneActorRegistryResetView *s)
{
    if (s->field2B2) {
        s->field2B2 = 0;
        return 1;
    }
    return 0;
}
void sub_080ACEE8(struct SceneActorRegistryResetView *s)
{
    struct DelayedAnimationContext *context = s->context;
    if (TakeEntryFlag(s))
        SceneActorAnimation_SelectDirectionalEntry((void *)s, 0);
    if (context->delay) {
        if (--context->delay == 0) {
            if (!(context->flags & 1))
                SceneActorAnimation_SelectDirectionalEntry((void *)s, 14);
            else
                SceneActorAnimation_SelectDirectionalEntryFlags((void *)s, 14, 4);
        }
    } else if (s->field2B3) {
        SceneActorAnimation_SelectDirectionalEntry((void *)s, 0);
        s->field2B0 = 1;
    }
}
