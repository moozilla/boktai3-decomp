#define SceneActorControl_UpdateRandomDelay sub_0809FC74
#define gRandomIndex                        gUnk_03005308
#define gRandomValues                       gUnk_0203B400
#include "global.h"
#include "scene_actor_control_phase.h"
static inline u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u8 HasFlag(struct SceneActorControlState *s)
{
    u32 mask = 4;
    if (s->flags3D4 & mask)
        return 1;
    return 0;
}
static inline u32 Random(void)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)(offset + (u32)gUnk_0203B400);
}
void sub_0809FC74(struct SceneActorRegistryResetView *s)
{
    struct SceneActorControlState *other = s->context;
    if (TakeFlag(&s->field2B1)) {
        void (*fn)(void) = s->savedTarget;
        u32 zero = 0;
        s->field2B2 = 1;
        s->field2B0 = zero;
        s->field2B5 = zero;
        s->target = fn;
        s->field2C8 = zero;
        {
            u32 *flags = &s->flags114;
            s32 mask = -2;
            *flags &= mask;
        }
        s->word104 = zero;
        other->randomDelay3ED = (Random() & 0x7F) + 20;
    }
    if (!HasFlag(other)) {
        u8 zero = sub_0824923C(s, s->update270);
        if (!zero) {
            if ((s32)s->counter2AC > other->randomDelay3ED && s->field2B3) {
                u32 two = 2;
                s->state2A8 = zero;
                s->phase2A9 = zero;
                s->behavior2AA = two;
                s->counter2AC = zero;
                s->field2B1 = 1;
            } else {
                ++s->counter2AC;
            }
        }
    }
}
