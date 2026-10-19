#define SceneActor_BeginContextReaction  sub_080B197C
#define SceneActor_DispatchContextAction sub_080B3ADC
// Tentative scene-actor family names; behavior inferred from static accesses.
#include "global.h"
#include "scene_actor_reset.h"
extern const u8 gUnk_0824E230[];
void sub_080B3110(void *, u32, const u8 *, u32);
void sub_080B3ADC(void *, u32);
void sub_0807FC90(void);
static inline u8 HasFlag(u32 v, u32 mask)
{
    if (v & mask)
        return 1;
    return 0;
}
static inline u8 ContextReady(struct SceneActorRegistryResetView *q)
{
    u32 mask = ~8;
    u32 flags = mask & q->flags6D4;
    q->flags6D4 = flags;
    if (HasFlag(flags, 1) || q->field67C >= 0)
        return 1;
    return 0;
}
static inline void SetMotion(struct SceneActorReactionMotion *p, u32 mask)
{
    p->flags = mask | p->flags;
}
static inline void SetWord(u32 *p, u32 mask)
{
    *p = mask | *p;
}
static inline void ClearWord(u32 *p, u32 mask)
{
    *p &= ~mask;
}
void sub_080B197C(struct SceneActorRegistryResetView *p)
{
    struct SceneActorRegistryResetView *q = p->context;
    if (ContextReady(q)) {
        struct SceneActorReactionMotion *motion;
        sub_080B3110(p, 1, gUnk_0824E230, 0x279);
        motion = (void *)&p->fieldDC;
        SetMotion(motion, 8);
        SetWord(&p->flags2CC, 0x80);
        return;
    }
    {
        u32 flag;
        struct SceneActorReactionMotion *motion = (void *)&p->fieldDC;
        flag = HasFlag(motion->flags, 8);
        if (flag) {
            sub_080B3ADC(q, 0);
        } else {
            u32 one = 1;
            p->state2A8 = flag;
            p->phase2A9 = one;
            p->behavior2AA = flag;
            p->counter2AC = flag;
            p->field2B1 = one;
            {
                void *callback = sub_0807FC90;
                u32 mode = 0x10;
                p->field2B2 = one;
                p->field2B0 = flag;
                p->field2B5 = mode;
                p->target = callback;
            }
            p->field2C8 = flag;
            ClearWord(&p->flags114, 1);
            p->word104 = flag;
        }
        SetMotion(motion, 8);
    }
}
