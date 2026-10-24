#include "global.h"
#include "scene_actor_reset.h"
// confidence=low: copies the owner context's 0x64A timer into 0x648, marks the context flags
// (set bit 0, clear bit 1), enters behavior 15 and starts the group's other route.
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
void sub_0808AF38(struct SceneActorRegistryResetView *);

struct Flags {
    s16 bits : 16;
};

static inline void SetFlags(u16 bit, u16 *field)
{
    struct Flags *p = (void *)field;
    p->bits |= bit;
}

static inline void ClearFlags(u16 *field, u32 bit)
{
    struct Flags *p = (void *)field;
    p->bits &= ~bit;
}

static inline void Set32(u32 bit, u32 *field)
{
    *field |= bit;
}

static inline void SetState(struct SceneActorRegistryResetView *actor, u32 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void StartRoute(void *body, void *path, u8 *group)
{
    u32 index = group[0x83] ^ 1;
    u8 *routes = group + 0x81;
    SceneBody_StartRoute(body, path, routes[index], 0, 0);
}

void sub_0808B264(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    {
        u16 timer = *(u16 *)(context + 0x64A);
        ((struct Flags *)(context + 0x648))->bits = timer;
    }
    SetFlags(1, (u16 *)(context + 0x628));
    ClearFlags((u16 *)(context + 0x628), 2);
    Set32(4, &actor->flags2D0);
    SetState(actor, 15);
    StartRoute((u8 *)actor + 0x50, &actor->route, (u8 *)actor + 0x3D4);
    sub_0808AF38(actor);
    ClearFlags((u16 *)(context + 0x628), 2);
}
