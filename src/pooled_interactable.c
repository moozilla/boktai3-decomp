#include "pooled_interactable.h"
// MATCHING_CONCESSION: register-pinning

struct PooledInteractable *sub_08008A70(struct PooledInteractableManager *manager)
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
void sub_08008A98(struct PooledInteractable *slot, u32 state)
{
    u32 z = 0;
    slot->state = state;
    slot->counter = z;
    slot->entered = 1;
}

/* Returns 1 once after a state starts. */
u32 sub_08008AA4(struct PooledInteractable *slot)
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
void sub_08008ABC(struct SceneCollider *other, u32 unused, struct PooledInteractable *slot)
{
    struct PooledInteractable *c = slot;
    if (slot->state == 1) {
        u32 m = 4;
        if (other->contactFlags2 & m)
            sub_08008A98(slot, 3);
    } else if (slot->state == 2) {
        u32 m = 8;
        if (other->contactFlags2 & m)
            sub_08008A98(c, 4);
    }
}

s32 Breakable_InitSlot(struct PooledInteractableManager *manager, struct PooledInteractable *slot)
{
    u32 fill;
    struct SceneExtent dimensions, scale, offset;
    void *resource;
    void *drawable;
    void *collider;
    register struct SceneExtent *d asm("r5");
    slot->resource = manager->resources;
    resource = slot->resource;
    asm("" : "=m"(fill));
    {
        d = &dimensions;
        fill = 0;
        {
            u32 *source = &fill;
            struct SceneExtent *dest = d;
            asm("" : : "r"(source), "r"(dest));
            CpuSet(source, dest, 0x05000002);
        }
    }
    drawable = &slot->drawable;
    sub_08219918(drawable, resource, 0, 1, 2, 0, 60, d);
    Anim_SetKeyframes(drawable, slot->resource, 0, 1, 0);
    collider = &slot->collider;
    scale.x = 128;
    scale.y = 128;
    scale.z = 128;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    Collider_Init(collider, slot->id, 0x4001, 0, 0, &scale, &offset);
    Collider_SetCallback(collider, sub_08008ABC, slot);
    Collider_SetPosition(collider, &slot->drawable.position, 0);
    Collider_Refresh(collider);
    return 0;
}

/* Runs the slot's script with (id, state-dependent flag, arg). */
void sub_08008BD0(struct PooledInteractable *slot)
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

void sub_08008C1C(struct PooledInteractableManager *manager, struct PooledInteractable *slot)
{
    if (sub_08008AA4(slot)) {
        void *q = &slot->drawable;
        Anim_SetKeyframes(q, slot->resource, gUnk_086049CE[slot->kind][0], 1, 0);
        if (slot->kind != 2)
            sub_08219A80(q, gUnk_086049DA[slot->kind] + 1);
        else
            sub_08219A80(q, gUnk_086049DA[2]);
    }
}

void Breakable_Idle(struct PooledInteractableManager *manager, struct PooledInteractable *slot)
{
    if (sub_08008AA4(slot)) {
        void *drawable = &slot->drawable;
        Anim_SetKeyframes(drawable, slot->resource, gUnk_086049CE[slot->kind][1], 1, 0);
        sub_08219A80(drawable, gUnk_086049DA[slot->kind]);
    }
    if (slot->timeout >= 0 && slot->counter >= slot->timeout)
        sub_08008A98(slot, 4);
}

void Breakable_React(struct PooledInteractableManager *manager, struct PooledInteractable *slot)
{
    if (sub_08008AA4(slot)) {
        void *drawable = &slot->drawable;
        Anim_SetKeyframes(drawable, slot->resource, gUnk_086049CE[slot->kind][1], 1, 0);
        sub_08219A80(drawable, gUnk_086049DA[slot->kind]);
        slot->flash = 10;
        Sound_PlaySong(331);
        sub_08008BD0(slot);
    }
    if (slot->counter > 10)
        sub_08008A98(slot, 2);
}

/* Local adaptation: this function only matches when the position is read as packed bitfields. */
#define POSITION (*(struct SceneExtent *)&slot->drawable.position)

void Breakable_Activate(struct PooledInteractableManager *manager, struct PooledInteractable *slot)
{
    struct SceneExtent effect;
    if (sub_08008AA4(slot)) {
        void *drawable = &slot->drawable;
        Anim_SetKeyframes(drawable, slot->resource, gUnk_086049CE[slot->kind][0], 1, 0);
        slot->flash = 10;
        sub_08008BD0(slot);
        if (slot->kind != 2) {
            sub_08219A80(drawable, gUnk_086049DA[slot->kind] + 1);
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
        sub_08008A98(slot, 1);
}

#undef POSITION

/* Motion 1: steps along the route and moves the position by the step. */
void sub_08008F00(struct PooledInteractable *s)
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
            sub_0821E618(q);
            s->delay = sub_0821E7E4(q);
        }
        r->x += op->x;
        s->drawable.position.z += op->z;
    }
}

/* Motion 2: copies the follow target's position, plus the height offset. */
void sub_08008F64(struct PooledInteractable *s)
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
                    sub_08008F00(slot);
                else if (slot->motion == 2)
                    sub_08008F64(slot);
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
    gUnk_02000034 = 0;
}

s32 Breakable_InitManager(struct PooledInteractableManager *manager)
{
    s32 i;
    u32 bytes;
    struct PooledInteractable *slot;
    gUnk_02000034 = manager;
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

void *sub_08009180(void)
{
    void *p;
    if (gUnk_02000034 != 0)
        return (void *)gUnk_02000034;
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

static inline s32 cellIndex(struct SceneVec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline s32 groundHeight(struct SceneVec *position)
{
    s32 index = cellIndex(position), height;
    u8 *cell = sub_0821D64C(index, 1);
    u32 kind;
    if (cell)
        cell += 4;
    else
        cell = gUnk_030052F4->cells + 12 + index * 4;
    kind = *cell >> 4;
    height = (*cell & 15) << 8;
    switch (kind) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}

s32 Breakable_ScriptCreate(void)
{
    struct SceneVec position;
    struct PooledInteractableManager *manager = gUnk_02000034;
    struct PooledInteractable *slot;
    s32 placed;
    void *drawable;
    if (!manager)
        goto fail;
    asm("" : : "r"(manager));
    slot = sub_08008A70(manager);
    if (!slot)
        goto fail;
    slot->motion = 0;
    placed = 0;
    if (Script_SeekToKeyword('r')) {
        s32 route = Script_GetValue(), point = Script_GetValue(), mode = Script_GetValue();
        if (!sub_0821E5C4(&slot->route, route, point, mode))
            goto fail;
        slot->motion = 1;
        slot->delay = sub_0821E7E4(&slot->route);
        slot->speed = Script_GetValue();
        slot->range = Script_GetValue();
        sub_0821E780(&position, slot->route.current, 0);
        placed = 1;
    }
    if (Script_SeekToKeyword('t')) {
        s32 target = Script_GetValue();
        slot->offset = Script_GetValue();
        slot->target = sub_08103EF4(target);
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
    sub_08008A98(slot, Script_GetKeywordValue('m', 1));
    slot->flags = Script_GetKeywordValue('f', 0);
    slot->timeout = Script_GetKeywordValue('d', 120);
    slot->id = Script_GetKeywordValue('n', 0);
    slot->kind = Script_GetKeywordValue('D', 0);
    slot->resource = &manager->resources[slot->kind];
    slot->script = Script_GetKeywordValue('p', 0);
    slot->arg = Script_GetKeywordValue('a', 0);
    {
        u32 motion = slot->motion;
        register struct PooledResource **resource asm("r7") = &slot->resource;
        if (motion != 2)
            position.y = groundHeight(&position);
        if (Script_GetKeywordValue('M', 0))
            slot->region = sub_0822C034(&position);
        else
            slot->region = -1;
        slot->drawable.position = position;
        slot->drawable.flags &= ~1;
        if (slot->state == 1) {
            drawable = &slot->drawable;
            {
                struct PooledResource *res = *resource;
                Anim_SetKeyframes(drawable, res, gUnk_086049CE[slot->kind][0], 1, 0);
            }
            if (slot->kind != 2)
                sub_08219A80(drawable, gUnk_086049DA[slot->kind] + 1);
            else
                sub_08219A80(drawable, gUnk_086049DA[2]);
        } else {
            drawable = &slot->drawable;
            {
                struct PooledResource *res = *resource;
                Anim_SetKeyframes(drawable, res, gUnk_086049CE[slot->kind][1], 1, 0);
            }
            sub_08219A80(drawable, gUnk_086049DA[slot->kind]);
        }
        sub_08214424(drawable, 0);
        slot->active = 1;
        return 0;
    }
}

void Breakable_ScriptSetTimeout(void)
{
    struct PooledInteractableManager *manager = gUnk_02000034;
    if (manager) {
        s32 id = Script_GetKeywordValue('n', 0), timeout = Script_GetKeywordValue('d', -1);
        struct PooledInteractable *slot = manager->slots;
        u32 i;
        for (i = 0; i < manager->count; i++, slot++)
            if (slot->active == 1 && slot->id == id)
                slot->timeout = timeout;
    }
}

void sub_080094A0(void)
{
    struct PooledInteractableManager *g = gUnk_02000034;
    if (g != 0) {
        u32 id = Script_GetKeywordValue(0x6e, 0);
        s32 m1 = -1;
        s32 v = Script_GetKeywordValue(0x6d, m1);
        if (v != m1 && v <= 4) {
            struct PooledInteractable *e = g->slots;
            u32 i = 0;
            for (; i < g->count; i++, e++) {
                if (e->active == 1 && e->id == id)
                    sub_08008A98(e, v);
            }
        }
    }
}
