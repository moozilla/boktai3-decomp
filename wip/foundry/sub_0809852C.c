#define ActorVisibility_TickBlink sub_0809852C
#define gEngineFlags              gUnk_030053F4
#include "global.h"
#include "scene_actor_reset.h"
void ActorShadow_Disable(void *);
void ActorShadow_Enable(void *);
extern volatile u32 gUnk_030053F4;
struct ActorBlinkState {
    u8 unknown0[0x558];
    u32 flags;
    u8 unknown55C[0x56];
    u16 counter;
};
static inline u32 Flags16(struct SceneActorRegistryResetView *a, u32 m)
{
    u32 off = 0x2D4;
    return *(u16 *)((u8 *)a + off) & m;
}
static inline u32 Flags32(struct SceneActorRegistryResetView *a, u32 m)
{
    u32 off = 0x2D0;
    return *(u32 *)((u8 *)a + off) & m;
}
static inline u8 Unavailable(struct SceneActorRegistryResetView *a)
{
    if (a) {
        if (Flags16(a, 0x40) || Flags32(a, 0x20000) || a->health <= 0)
            return 1;
    }
    return 0;
}
static inline u8 Test(u32 v, u32 m)
{
    if (v & m)
        return 1;
    return 0;
}
static inline void Show(struct SceneActorRegistryResetView *a)
{
    *(u32 *)a->drawable &= ~1;
    {
        u32 offset = 0x320;
        ActorShadow_Enable((u8 *)a + offset);
    }
}
static inline void Hide(struct SceneActorRegistryResetView *a)
{
    *(u32 *)a->drawable |= 1;
    {
        u32 offset = 0x320;
        ActorShadow_Disable((u8 *)a + offset);
    }
}
void sub_0809852C(struct SceneActorRegistryResetView *a)
{
    struct ActorBlinkState *s;
    u32 counter, value;
    if (Unavailable(a))
        return;
    s = a->context;
    {
        u32 mask = 0x60;
        u32 off = 0x558;
        if (!Test(*(u32 *)((u8 *)s + off), mask))
            return;
    }
    (void)(gUnk_030053F4 & 0x4800);
    counter = (u32)s->counter << 16;
    value = counter >> 16;
    if (value <= 19) {
        if (({
                u32 shifted = counter >> 18;
                u16 one = 1;
                shifted & one;
            }))
            goto show;
        else
            goto hide;
    } else if (value <= 29) {
        if (({
                u32 shifted = counter >> 17;
                u16 one = 1;
                shifted & one;
            })) {
        show:
            Show(a);
        } else {
        hide:
            Hide(a);
        }
    } else {
        u32 mask = 0x20;
        u32 *flags = &s->flags;
        u32 active = *flags & mask;
        if (Test(active, ~0)) {
            *(u32 *)a->drawable &= ~1;
            {
                u32 mask = ~0x20;
                *flags &= mask;
            }
            ActorShadow_Enable((u8 *)a + 0x320);
        } else {
            *(u32 *)a->drawable |= 1;
            {
                u32 mask = ~0x40;
                *flags &= mask;
            }
            ActorShadow_Disable((u8 *)a + 0x320);
        }
    }
    s->counter++;
}
