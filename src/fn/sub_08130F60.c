#include "scene_actor_control_phase.h"
s32 Callback_InvokeActor(void *arg, void *function);
s32 Route_GetCurrentPointNodeId(void *);
u8 sub_0812F3CC(void *);
void SceneActorRoute_AdvanceActorPoint(void *, void *);
static inline u32 tst(u32 *a, u32 m)
{
    return *a & m;
}
static inline void setf(u32 *a, u32 m)
{
    *a |= m;
}
void sub_08130F60(u8 *s)
{

    u8 *p = s + 0x448;
    if (SceneActorControl_TakeFlag(s + 0x2B1)) {
        struct ChaseActor *actor = (void *)s;
        void *target = actor->base.onEntry;
        u32 kind = 2;
        actor->base.field2B2 = 1;
        actor->base.stepDone = 0;
        actor->base.controlKind = kind;
        actor->base.target = target;
        actor->base.field2C8 = 0;
        {
            u32 *flags = &actor->base.pendingControlFlags;
            s32 mask = ~1;
            *flags &= mask;
        }
        actor->base.pendingControl = 0;
    }
    if (!(u8)Callback_InvokeActor(s, *(void **)(s + 0x270))) {
        if (!sub_0812F3CC(s)) {
            u8 c = s[0x2B0];
            u32 *f = (u32 *)(s + 0x2D0);
            if (c)
                setf(f, 0x20);
            if (tst(f, 0x20)) {
                if (p[3]++ >= Route_GetCurrentPointNodeId(p)) {
                    SceneActorRoute_AdvanceActorPoint(s, p);
                    return;
                }
            }
            ++*(u32 *)(s + 0x2AC);
        }
    }
}
