#include "global.h"
#include "scene_actor_reset.h"
#include "engine.h"
#include "debug.h"
struct ActorRevealState {
    u8 unknown0[0x5BB];
    u8 revealTicks;
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
void ActorVisibility_TickRevealDelay(struct SceneActorRegistryResetView *a)
{
    struct ActorRevealState *s = a->context;
    if (Unavailable(a))
        return;
    if (Test(gEngineFlags, 0x4800)) {
        DEBUG_PRINT();
    }
    if (s->revealTicks) {
        *(u32 *)a->drawable &= ~1;
        s->revealTicks--;
    }
}
