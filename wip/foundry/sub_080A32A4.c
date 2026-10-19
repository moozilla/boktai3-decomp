#define SceneActorControl_UpdateResponseCompletion sub_080A32A4
#include "global.h"
#include "scene_actor_reset.h"
s32 sub_0824923C(void *, void *);
static inline u8 TakeEntry(struct SceneActorRegistryResetView *s)
{
    if (s->field2B1) {
        s->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void ResetFlags(struct SceneActorRegistryResetView *s, u32 zero)
{
    u32 *flags = &s->flags114;
    s32 mask = -2;
    *flags &= mask;
    s->word104 = zero;
}
void sub_080A32A4(struct SceneActorRegistryResetView *s)
{
    if (TakeEntry(s)) {
        void *fn = s->entryTarget224;
        u32 two = 2;
        u8 *ready = &s->field2B2;
        u32 zero = 0;
        *ready = 1;
        {
            u32 offset = 0x2B0;
            ((u8 *)s)[offset] = zero;
            offset += 5;
            ((u8 *)s)[offset] = two;
            s->target = fn;
            offset += 0x13;
            *(u16 *)((u8 *)s + offset) = zero;
        }
        ResetFlags(s, zero);
    } else {
        u32 zero = (u8)sub_0824923C(s, s->update270);
        if (zero)
            return;
        if (!s->field2B0)
            goto increment;
        {
            u32 seven = 7;
            u32 offsetA = 0x2A8;
            u32 offsetB = 0x2B0;
            void *fn;
            ((u8 *)s)[offsetA] = zero;
            offsetB -= 7;
            ((u8 *)s)[offsetB] = zero;
            offsetA += 2;
            ((u8 *)s)[offsetA] = seven;
            offsetB += 3;
            *(u32 *)((u8 *)s + offsetB) = zero;
            s->field2B1 = 1;
            offsetA -= 0x82;
            fn = *(void **)((u8 *)s + offsetA);
            offsetB += 6;
            ((u8 *)s)[offsetB] = 1;
            offsetB -= 2;
            ((u8 *)s)[offsetB] = zero;
            offsetB += 5;
            ((u8 *)s)[offsetB] = 1;
            s->target = fn;
            s->field2C8 = zero;
            ResetFlags(s, zero);
        }
    }
    return;
increment:
    ++s->counter2AC;
}
