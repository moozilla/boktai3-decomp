#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_route.h"

// confidence=low: when the idle flag is set and the delay expires, picks a random route point and starts moving to it.

struct GhostMoveContext {
    u8 unknown0[0x558];
    u32 flags558;
    u8 unknown55C[0x60C - 0x55C];
    struct SceneActorRouteCursor route;
    u8 unknown618[0x624 - 0x618];
    u16 delay;
};
u32 Ghost_SelectRandomRouteEntry(struct SceneActorRouteCursor *);
void Ghost_StartMovementToPosition(void *, void *);
void Bee_ClearActionFlags(void *);
void SceneActor_EndLinkedReaction(void *);
static inline u8 Blocked(struct SceneActorRegistryResetView *s)
{
    if (!s)
        goto zero;
    {
        u32 mask = 0x40;
        if (s->flags2D4 & mask)
            goto one;
    }
    {
        u32 mask = 0x20000;
        if (s->flags2D0 & mask)
            goto one;
    }
    if (s->health > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}
static inline u16 TestFlag(struct SceneActorRegistryResetView *p, u32 mask)
{
    mask &= p->flags2D4;
    return mask;
}
static inline u8 HasFlag(struct GhostMoveContext *c, u32 mask)
{
    if (c->flags558 & mask)
        return 1;
    return 0;
}
void Ghost_UpdateRandomDestination(struct SceneActorRegistryResetView *actor)
{
    struct GhostMoveContext *state = actor->context;
    if (Blocked(actor))
        return;
    if (TestFlag(actor, 0x120))
        return;
    {
        u8 ready = HasFlag(state, 0x10);
        if (!ready) {
            state->delay = ready;
        } else if (state->delay) {
            state->delay--;
        } else {
            struct ContactPosition target;
            struct ContactPosition *p;
            struct SceneActorRouteCursor *route = &state->route;
            s32 cell;
            u16 attr;
            u32 kind;
            s32 height;
            u32 zero, two;
            Ghost_SelectRandomRouteEntry(route);
            p = &target;
            p->x = route->point->x;
            p->z = route->point->z;
            cell = SceneActorTerrain_GetCellIndex(p);
            attr = *SceneActorTerrain_GetCell(cell);
            kind = attr >> 4;
            {
                u32 mask = 15;
                mask &= attr;
                height = mask << 8;
            }
            switch (kind) {
                case 1:
                    height -= ((u8 *)p)[4];
                    break;
                case 2:
                    height -= ((u8 *)p)[0];
                    break;
            }
            p->y = height;
            zero = 0;
            Ghost_StartMovementToPosition(actor, &target);
            Bee_ClearActionFlags(actor);
            SceneActor_EndLinkedReaction(actor);
            two = 2;
            actor->state2A8 = zero;
            actor->phase2A9 = zero;
            actor->behavior2AA = two;
            actor->counter2AC = zero;
            actor->field2B1 = 1;
        }
    }
}
