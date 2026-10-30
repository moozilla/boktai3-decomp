#include "scene_actor_chase.h"
// confidence=low: reacts to the context probe flag and enters behavior 15 after clearing it.
void sub_0808AF38(struct SceneActorRegistryResetView *);
static inline u8 TakeEntry(struct SceneActorRegistryResetView *a)
{
    if (a->field2B2) {
        a->field2B2 = 0;
        return 1;
    }
    return 0;
}
struct Bits16 {
    s16 bits : 16;
};
static inline u8 Test(u8 *context, u32 mask, struct Bits16 **out)
{
    struct Bits16 *p = (void *)(context + 0x628);
    u32 value = *(u16 *)p & mask;
    *out = p;
    if (value)
        return 1;
    return 0;
}
static inline void Clear(struct Bits16 *p, s32 mask)
{
    p->bits = mask & p->bits;
}
void Bat_UpdateProbeReaction(struct SceneActorRegistryResetView *a)
{
    u8 *context = a->context;
    struct Bits16 *flags;
    if (TakeEntry(a))
        return;
    if (Test(context, 2, &flags)) {
        if (a->tracking.distance > 0xFFFF)
            SceneActorAnimation_SelectDirectionalEntry(a, 0);
        sub_0808AF38(a);
        Clear(flags, ~2);
        Chase_SetBehavior(a, 15);
    } else
        SceneActorAnimation_SelectDirectionalEntry(a, 1);
}
