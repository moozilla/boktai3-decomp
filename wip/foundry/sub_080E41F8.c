#include "global.h"
#include "scene_actor_reset.h"
#define SceneActor_BeginAnimationWait sub_080E41F8
void sub_080E1984(void);
void sub_080DF400(struct SceneActorRegistryResetView *);
void sub_0821FF58(void *, s32, s32, s32, s32, s32);
static inline u32 Mask(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline u16 Or(u16 mask, u16 *p)
{
    return mask | *p;
}
static inline u16 Mask16(u32 mask, u16 *p)
{
    return mask & *p;
}
static inline u8 TakeEntry(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
void sub_080E41F8(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRegistryResetView *ctx = actor->context;
    if (TakeEntry(&actor->field2B1)) {
        ctx->animationFlags678 = Mask(&ctx->animationFlags678, ~8);
        actor->flags2D4 = Or(0x100, &actor->flags2D4);
        {
            u8 zero = 0;
            u16 zero16 = 0;
            void *callback = sub_080E1984;
            u8 kind = 0x22;
            actor->field2B2 = 1;
            actor->field2B0 = zero;
            actor->field2B5 = kind;
            actor->target = callback;
            actor->field2C8 = zero16;
            actor->flags114 = Mask(&actor->flags114, ~1);
            actor->word104 = zero16;
            {
                struct SceneActorRegistryResetView *p = actor->context;
                *(u32 *)((u8 *)p + 0x678) = Mask((u32 *)((u8 *)p + 0x678), ~2);
            }
            sub_0821FF58(ctx->animation624, actor->fieldB6, actor->fieldBE, 0, actor->wordD0,
                         actor->fieldC6);
            ctx->animationFlag6A7 = zero;
            ctx->field698 = ctx->animationFrame69C;
        }
    }
    if (actor->field2B0) {
        ctx->field698 = ctx->animationFrame69C;
        actor->flags2D4 = Mask16(~0x100, &actor->flags2D4);
        sub_080DF400(actor);
    } else {
        actor->counter2AC++;
    }
}
