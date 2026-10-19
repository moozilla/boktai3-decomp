#define SceneActorControl_FinishDirectionalRecovery sub_080A2A38
#include "global.h"
#include "scene_actor_reset.h"
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void sub_080A2A38(struct SceneActorRegistryResetView *s)
{
    u32 group = (u32)s + 0x3D4;
    u32 remaining;
    if (s->field2B2)
        s->field2B2 = 0;
    SceneActorAnimation_SelectDirectionalEntry(s, 1);
    remaining = *(u16 *)(group + 0x44);
    if (!remaining) {
        u16 *flags = &s->flags2D4;
        s32 mask = -0x81;
        u32 zero;
        u32 seven;
        void *fn;
        zero = 0;
        *flags &= mask;
        seven = 7;
        {
            u32 offsetA = 0x2A8;
            u32 offsetB;
            ((u8 *)s)[offsetA] = zero;
            offsetB = 0x2A9;
            ((u8 *)s)[offsetB] = zero;
            offsetA += 2;
            ((u8 *)s)[offsetA] = seven;
            s->counter2AC = remaining;
            offsetB += 8;
            ((u8 *)s)[offsetB] = 1;
            offsetA -= 0x82;
            fn = *(void **)((u8 *)s + offsetA);
            offsetA += 0x8A;
            ((u8 *)s)[offsetA] = 1;
            offsetA -= 2;
            ((u8 *)s)[offsetA] = zero;
            {
                u32 one = 1;
                u32 offset = 0x2B5;
                ((u8 *)s)[offset] = one;
            }
            offsetA -= 0x94;
            *(void **)((u8 *)s + offsetA) = fn;
        }
        s->field2C8 = remaining;
        {
            u32 *flags = &s->flags114;
            s32 mask = -2;
            *flags &= mask;
        }
        s->word104 = remaining;
    }
}
