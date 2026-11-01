#include "pooled_interactable.h"
#include "scene_actor_terrain.h"
/* Breakable_NullCallback: Empty no-op function used as a placeholder/gap callback entry. */
#define Breakable_NullCallback sub_08008C18

struct PooledInteractable *Breakable_FindFreeSlot(struct PooledInteractableManager *manager)
{
    struct PooledInteractable *slot = manager->slots;
    u32 i;
    for (i = 0; i < manager->count; i++, slot++) {
        if (slot->active == 0)
            return slot;
    }
    return 0;
}

/* Starts a new state. */
void Breakable_EnterState(struct PooledInteractable *slot, u32 state)
{
    u32 z = 0;
    slot->state = state;
    slot->counter = z;
    slot->entered = 1;
}

/* Returns 1 once after a state starts. */
u32 Breakable_ConsumeEnteredFlag(struct PooledInteractable *slot)
{
    u32 r;
    if (slot->entered == 0) {
        r = 0;
    } else {
        slot->entered = 0;
        r = 1;
    }
    return r;
}

/* Collider callback: moves to state 3 or 4 when hit by the matching attack kind. */
void Breakable_OnHit(struct SceneCollider *other, u32 unused, struct PooledInteractable *slot)
{
    struct PooledInteractable *c = slot;
    if (slot->state == 1) {
        u32 m = 4;
        if (other->contactFlags2 & m)
            Breakable_EnterState(slot, 3);
    } else if (slot->state == 2) {
        u32 m = 8;
        if (other->contactFlags2 & m)
            Breakable_EnterState(c, 4);
    }
}

s32 Breakable_InitSlot(struct PooledInteractableManager *manager, struct PooledInteractable *slot)
{
    struct SceneExtent dimensions, scale, offset;
    void *resource;
    void *drawable;
    void *collider;
    slot->resource = manager->resources;
    resource = slot->resource;
    ZeroFill8(&dimensions);
    drawable = &slot->drawable;
    Widget_InitCleared(drawable, resource, 0, 1, 2, 0, 60, &dimensions);
    Anim_SetKeyframes(drawable, slot->resource, 0, 1, 0);
    collider = &slot->collider;
    scale.x = 128;
    scale.y = 128;
    scale.z = 128;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    Collider_Init(collider, slot->id, 0x4001, 0, 0, &scale, &offset);
    Collider_SetCallback(collider, Breakable_OnHit, slot);
    Collider_SetPosition(collider, &slot->drawable.position, 0);
    Collider_Refresh(collider);
    return 0;
}

/* Runs the slot's script with (id, state-dependent flag, arg). */
void Breakable_RunScript(struct PooledInteractable *slot)
{
    u32 a[3];
    struct PooledPackedArgs b;
    if (slot->script != 0) {
        a[0] = slot->id;
        if (slot->state == 3)
            a[1] = 1;
        else if (slot->state == 4)
            a[1] = 0;
        a[2] = slot->arg;
        b.flags = 3;
        b.args = a;
        Script_Call(slot->script, &b);
    }
}

/* Empty callback slot (gap function). */
void sub_08008C18(void)
{
}

void Breakable_UpdateAnimation(struct PooledInteractableManager *manager,
                               struct PooledInteractable *slot)
{
    if (Breakable_ConsumeEnteredFlag(slot)) {
        void *q = &slot->drawable;
        Anim_SetKeyframes(q, slot->resource, gUnk_086049CE[slot->kind][0], 1, 0);
        if (slot->kind != 2)
            Anim_SelectPaletteBank(q, gUnk_086049DA[slot->kind] + 1);
        else
            Anim_SelectPaletteBank(q, gUnk_086049DA[2]);
    }
}

void Breakable_Idle(struct PooledInteractableManager *manager, struct PooledInteractable *slot)
{
    if (Breakable_ConsumeEnteredFlag(slot)) {
        void *drawable = &slot->drawable;
        Anim_SetKeyframes(drawable, slot->resource, gUnk_086049CE[slot->kind][1], 1, 0);
        Anim_SelectPaletteBank(drawable, gUnk_086049DA[slot->kind]);
    }
    if (slot->timeout >= 0 && slot->counter >= slot->timeout)
        Breakable_EnterState(slot, 4);
}

void Breakable_React(struct PooledInteractableManager *manager, struct PooledInteractable *slot)
{
    if (Breakable_ConsumeEnteredFlag(slot)) {
        void *drawable = &slot->drawable;
        Anim_SetKeyframes(drawable, slot->resource, gUnk_086049CE[slot->kind][1], 1, 0);
        Anim_SelectPaletteBank(drawable, gUnk_086049DA[slot->kind]);
        slot->flash = 10;
        Sound_PlaySong(331);
        Breakable_RunScript(slot);
    }
    if (slot->counter > 10)
        Breakable_EnterState(slot, 2);
}

/* Local adaptation: this function only matches when the position is read as packed bitfields. */
#define POSITION (*(struct SceneExtent *)&slot->drawable.position)

void Breakable_Activate(struct PooledInteractableManager *manager, struct PooledInteractable *slot)
{
    struct SceneExtent effect;
    if (Breakable_ConsumeEnteredFlag(slot)) {
        void *drawable = &slot->drawable;
        Anim_SetKeyframes(drawable, slot->resource, gUnk_086049CE[slot->kind][0], 1, 0);
        slot->flash = 10;
        Breakable_RunScript(slot);
        if (slot->kind != 2) {
            Anim_SelectPaletteBank(drawable, gUnk_086049DA[slot->kind] + 1);
            effect = POSITION;
            effect.x = POSITION.x;
            effect.y = POSITION.y + 400;
            effect.z = POSITION.z;
            AnimatedParticles_EmitDirectional(1, 2, 0x2001, 64, 0, 0, 0, 0, 4, 2, &effect);
            effect.x = POSITION.x - 64;
            effect.y = POSITION.y + 192;
            effect.z = POSITION.z + 64;
            AnimatedParticles_EmitDirectional(1, 2, 0x2001, 64, 0, 0, 0, 0, 4, 2, &effect);
            effect.x = POSITION.x + 64;
            effect.y = POSITION.y + 192;
            effect.z = POSITION.z - 64;
            AnimatedParticles_EmitDirectional(1, 2, 0x2001, 64, 0, 0, 0, 0, 4, 2, &effect);
        } else {
            effect = POSITION;
            effect.x = POSITION.x;
            effect.y = POSITION.y + 200;
            effect.z = POSITION.z;
            AnimatedParticles_EmitDirectional(1, 2, 0x2001, 64, 0, 0, 0, 0, 4, 2, &effect);
        }
    }
    if (slot->counter > 10)
        Breakable_EnterState(slot, 1);
}

#undef POSITION

/* Motion 1: steps along the route and moves the position by the step. */
void Breakable_UpdateRouteMotion(struct PooledInteractable *s)
{
    void *q = &s->route;
    if (s->delay != 0) {
        s->delay--;
    } else {
        struct SceneVec *r = &s->drawable.position;
        struct SceneExtent *op;
        struct {
            u32 w;
            struct SceneExtent o;
        } m;
        op = &m.o;
        if ((u8)sub_0821E67C(q, r, op, &m.w, s->range, s->range, s->speed)) {
            Route_AdvancePoint(q);
            s->delay = Route_GetCurrentPointNodeId(q);
        }
        r->x += op->x;
        s->drawable.position.z += op->z;
    }
}

/* Motion 2: copies the follow target's position, plus the height offset. */
void Breakable_UpdateFollowMotion(struct PooledInteractable *s)
{
    struct SceneVec *r = s->target;
    if (r != 0) {
        s->drawable.position.x = r->x;
        s->drawable.position.y = r->y + s->offset;
        s->drawable.position.z = r->z;
    }
}

s32 Breakable_UpdateAll(struct PooledInteractableManager *manager)
{
    struct PooledInteractable *slot = manager->slots;
    u32 i;
    for (i = 0; i < manager->count; i++, slot++) {
        if (slot->active) {
            if (slot->region >= 0 && !sameRegion(slot->region)) {
                slot->drawable.flags |= 1;
                slot->collider.state |= 4;
            } else {
                slot->drawable.flags &= ~1;
                slot->collider.state &= ~4;
                sub_08249240(manager, slot, gUnk_086049E0[slot->state]);
                if (slot->motion == 1)
                    Breakable_UpdateRouteMotion(slot);
                else if (slot->motion == 2)
                    Breakable_UpdateFollowMotion(slot);
                Anim_Step(&slot->drawable, slot->resource);
                Collider_SetPosition(&slot->collider, &slot->drawable.position, 0);
                if (slot->flash) {
                    slot->flash--;
                    if (!slot->flash)
                        slot->collider.state &= ~4;
                    else
                        slot->collider.state |= 4;
                }
                slot->counter++;
            }
        }
    }
}

s32 Breakable_DestroyManager(struct PooledInteractableManager *manager)
{
    struct PooledInteractable *slot = manager->slots;
    u32 i;
    for (i = 0; i < manager->count; i++, slot++)
        if (slot->active) {
            Collider_Remove(&slot->collider);
            Anim_Remove(&slot->drawable);
        }
    if (manager->slots) {
        Heap_Free(manager->slots);
        manager->slots = 0;
    }
    gPooledInteractableManager = 0;
}

s32 Breakable_InitManager(struct PooledInteractableManager *manager)
{
    s32 i;
    u32 bytes;
    struct PooledInteractable *slot;
    gPooledInteractableManager = manager;
    for (i = 0; i < 3; i++) {
        struct PooledResource *resource = ScriptProc_Start(0xcb05, gUnk_086049C8[i]);
        if (!resource)
            return -1;
        manager->resources[i] = *resource;
        ResourceHeader_Relocate(&manager->resources[i], resource);
    }
    manager->count = Script_GetKeywordValue(110, 8);
    bytes = manager->count * sizeof(struct PooledInteractable);
    manager->slots = Heap_Alloc(bytes);
    Memory_Clear(manager->slots, bytes);
    slot = manager->slots;
    for (i = 0; (u32)i < manager->count; i++, slot++)
        Breakable_InitSlot(manager, slot);
}

void *Breakable_GetOrCreateManager(void)
{
    void *p;
    if (gPooledInteractableManager != 0)
        return (void *)gPooledInteractableManager;
    p = Task_Create(10, 0x80);
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void))Breakable_UpdateAll,
                          (void (*)(void))Breakable_DestroyManager);
        if (Breakable_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

static inline s32 groundHeight(struct SceneVec *position)
{
    const struct ContactPosition *p = (const struct ContactPosition *)position;
    return SceneActorTerrain_GetHeight(SceneActorTerrain_GetCellIndex(p), p);
}

s32 Breakable_ScriptCreate(void)
{
    struct SceneVec position;
    struct PooledInteractableManager *manager = gPooledInteractableManager;
    struct PooledInteractable *slot;
    s32 placed;
    if (!manager)
        return -1;
    slot = Breakable_FindFreeSlot(manager);
    if (!slot)
        goto fail;
    slot->motion = 0;
    placed = 0;
    if (Script_SeekToKeyword('r')) {
        s32 route = Script_GetValue(), point = Script_GetValue(), mode = Script_GetValue();
        if (!Route_Start(&slot->route, route, point, mode))
            goto fail;
        slot->motion = 1;
        slot->delay = Route_GetCurrentPointNodeId(&slot->route);
        slot->speed = Script_GetValue();
        slot->range = Script_GetValue();
        Path_GetPointPosition(&position, slot->route.current, 0);
        placed = 1;
    }
    if (Script_SeekToKeyword('t')) {
        s32 target = Script_GetValue();
        slot->offset = Script_GetValue();
        slot->target = NamedObject_GetPositionByName(target);
        if (!slot->target)
            goto fail;
        slot->motion = 2;
        position = *slot->target;
        placed = 1;
    }
    if (Script_SeekToKeyword('P')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
        placed = 1;
    }
    if (!placed) {
    fail:
        return -1;
    }
    Breakable_EnterState(slot, Script_GetKeywordValue('m', 1));
    slot->flags = Script_GetKeywordValue('f', 0);
    slot->timeout = Script_GetKeywordValue('d', 120);
    slot->id = Script_GetKeywordValue('n', 0);
    slot->kind = Script_GetKeywordValue('D', 0);
    slot->resource = &manager->resources[slot->kind];
    slot->script = Script_GetKeywordValue('p', 0);
    slot->arg = Script_GetKeywordValue('a', 0);
    if (slot->motion != 2)
        position.y = groundHeight(&position);
    if (Script_GetKeywordValue('M', 0))
        slot->region = Region_FindContaining(&position);
    else
        slot->region = -1;
    slot->drawable.position = position;
    slot->drawable.flags &= ~1;
    if (slot->state == 1) {
        Anim_SetKeyframes(&slot->drawable, slot->resource, gUnk_086049CE[slot->kind][0], 1, 0);
        if (slot->kind != 2)
            Anim_SelectPaletteBank(&slot->drawable, gUnk_086049DA[slot->kind] + 1);
        else
            Anim_SelectPaletteBank(&slot->drawable, gUnk_086049DA[2]);
    } else {
        Anim_SetKeyframes(&slot->drawable, slot->resource, gUnk_086049CE[slot->kind][1], 1, 0);
        Anim_SelectPaletteBank(&slot->drawable, gUnk_086049DA[slot->kind]);
    }
    Anim_LinkToList(&slot->drawable, 0);
    slot->active = 1;
    return 0;
}

void Breakable_ScriptSetTimeout(void)
{
    struct PooledInteractableManager *manager = gPooledInteractableManager;
    if (manager) {
        s32 id = Script_GetKeywordValue('n', 0), timeout = Script_GetKeywordValue('d', -1);
        struct PooledInteractable *slot = manager->slots;
        u32 i;
        for (i = 0; i < manager->count; i++, slot++)
            if (slot->active == 1 && slot->id == id)
                slot->timeout = timeout;
    }
}

void Breakable_ScriptSetState(void)
{
    struct PooledInteractableManager *g = gPooledInteractableManager;
    if (g != 0) {
        u32 id = Script_GetKeywordValue(0x6e, 0);
        s32 m1 = -1;
        s32 v = Script_GetKeywordValue(0x6d, m1);
        if (v != m1 && v <= 4) {
            struct PooledInteractable *e = g->slots;
            u32 i = 0;
            for (; i < g->count; i++, e++) {
                if (e->active == 1 && e->id == id)
                    Breakable_EnterState(e, v);
            }
        }
    }
}
