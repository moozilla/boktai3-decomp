#include "terrain_modifier.h"

void TerrainModifiers_ClearGlobal(void)
{
    gTerrainModifierManager = 0;
}

u32 TerrainModifiers_AddToList(struct TerrainModifier *p)
{
    struct TerrainModifier *t;
    p->previous = 0;
    t = gTerrainModifierManager->head;
    p->next = t;
    if (t) {
        t->previous = p;
    }
    gTerrainModifierManager->head = p;
    return 0;
}

u32 TerrainModifiers_RemoveFromList(struct TerrainModifier *p)
{
    struct TerrainModifier *prev = p->previous;
    struct TerrainModifier *next = p->next;
    if (prev) {
        prev->next = next;
    } else {
        gTerrainModifierManager->head = next;
    }
    if (next) {
        next->previous = prev;
    }
    return 0;
}

static inline s32 heightLevel(s32 value)
{
    if (value >= 0)
        value >>= 8;
    else
        value = -((-value) >> 8);
    return value;
}

static inline s32 cellIndex(struct SceneVec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

s32 TerrainModifiers_Create(s32 id, struct SceneVec *position, s32 kind, s32 shape, s32 flags)
{
    struct TerrainModifier *p;
    if (!gTerrainModifierManager && !TerrainModifierManager_Create(0x2153, 0))
        return 0;
    p = Heap_Alloc(sizeof(*p));
    if (!p)
        return -1;
    Memory_Clear(p, sizeof(*p));
    p->id = id;
    p->active = 1;
    p->kind = kind;
    p->shape = shape;
    p->cell = cellIndex(position);
    p->height = heightLevel(position->y);
    p->flags = flags;
    TerrainModifiers_RegisterCell(&p->registration, p->cell, p->shape, p->height, 255, p->flags);
    TerrainModifiers_AddToList(p);
    return 0;
}

void TerrainModifiers_UnregisterCell(struct TerrainCellOverride *);

u32 TerrainModifiers_Destroy(struct TerrainModifier *n)
{
    if (n->active != 0) {
        TerrainModifiers_UnregisterCell(&n->registration);
        n->active = 0;
    }
    if (gTerrainModifierManager != 0)
        TerrainModifiers_RemoveFromList(n);
    Heap_Free(n);
    return 0;
}

s32 TerrainModifiers_DestroyAll(void)
{
    struct TerrainModifier *n;
    if (gTerrainModifierManager == 0)
        return -1;
    n = gTerrainModifierManager->head;
    while (n != 0) {
        struct TerrainModifier *next = n->next;
        TerrainModifiers_Destroy(n);
        n = next;
    }
    return 0;
}

s32 TerrainModifiers_DestroyById(u16 id)
{
    struct TerrainModifier *n;
    if (gTerrainModifierManager == 0)
        return -1;
    n = gTerrainModifierManager->head;
    while (n != 0) {
        struct TerrainModifier *next = n->next;
        if (n->id == id)
            TerrainModifiers_Destroy(n);
        n = next;
    }
    return 0;
}

s32 TerrainModifiers_Deactivate(u16 id)
{
    struct TerrainModifier *n;
    if (gTerrainModifierManager == 0)
        return -1;
    n = gTerrainModifierManager->head;
    while (n != 0) {
        struct TerrainModifier *next = n->next;
        if (n->id == id && n->active != 0) {
            n->active = 0;
            TerrainModifiers_UnregisterCell(&n->registration);
        }
        n = next;
    }
    return 0;
}

s32 TerrainModifiers_Activate(u32 id)
{
    struct TerrainModifier *e;
    struct TerrainModifier *n;
    u16 k = id;
    if (gTerrainModifierManager == 0)
        return -1;
    e = gTerrainModifierManager->head;
    while (e != 0) {
        n = e->next;
        if (e->id == k && e->active == 0) {
            e->active = 1;
            TerrainModifiers_RegisterCell(&e->registration, e->cell, e->shape, e->height, 0xff,
                                          e->flags);
        }
        e = n;
    }
    return 0;
}

void TerrainModifiers_InitCell(struct TerrainCellOverride *, u32, u32, u32, u32, u32);

s32 TerrainModifiers_Change(s32 id, s32 shape, s32 kind, s32 flags, s32 cell, s32 height)
{
    struct TerrainModifier *p;
    if (!gTerrainModifierManager)
        return -1;
    p = gTerrainModifierManager->head;
    while (p) {
        struct TerrainModifier *next = p->next;
        if (p->id == id) {
            if (kind != -1)
                p->kind = kind;
            if (shape != -1)
                p->shape = shape;
            if (cell != -1)
                p->cell = cell;
            if (height != -1)
                p->height = height;
            if (flags != -1)
                p->flags = flags;
            TerrainModifiers_InitCell(&p->registration, p->cell, p->shape, p->height, 255,
                                      p->flags);
        }
        p = next;
    }
    return 0;
}

u32 TerrainModifierManager_Update(void)
{
    return 0;
}

u32 TerrainModifierManager_Destroy(void)
{
    TerrainModifiers_DestroyAll();
    gTerrainModifierManager = 0;
    return 0;
}

s32 TerrainModifierManager_Init(struct TerrainModifierManager *p, s32 unused)
{
    gTerrainModifierManager = p;
    p->head = 0;
    return 0;
}

void *TerrainModifierManager_Create(s32 a, s32 unused)
{
    void *p = Task_Create(8, sizeof(struct TerrainModifierManager));
    if (p != 0) {
        Task_SetCallbacks(p, TerrainModifierManager_Update, TerrainModifierManager_Destroy);
        if (TerrainModifierManager_Init(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

u32 TerrainModifiers_DestroyFromScript(void)
{
    return TerrainModifiers_DestroyById((u16)Script_GetKeywordValue(0x6e, 0));
}

u32 TerrainModifiers_DeactivateFromScript(void)
{
    return TerrainModifiers_Deactivate((u16)Script_GetKeywordValue(0x6e, 0));
}

u32 TerrainModifiers_ActivateFromScript(void)
{
    return TerrainModifiers_Activate((u16)Script_GetKeywordValue(0x6e, 0));
}

s32 TerrainModifiers_ChangeFromScript(void)
{
    s32 id = Script_GetKeywordValue('n', -1), shape = Script_GetKeywordValue('t', -1),
        kind = Script_GetKeywordValue('f', -1), flags = Script_GetKeywordValue('e', -1);
    struct SceneVec position;
    s32 cell;
    s32 height;
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
        cell = cellIndex(&position);
        height = heightLevel(position.y);
    } else {
        height = -1;
        cell = height;
    }
    return TerrainModifiers_Change(id, shape, kind, flags, cell, height);
}

u32 TerrainModifiers_CreateFromScript(void)
{
    struct SceneVec position;
    s32 id = Script_GetKeywordValue('n', 0);
    s32 shape = Script_GetKeywordValue('t', 0);
    s32 kind = Script_GetKeywordValue('f', 0);
    s32 flags = Script_GetKeywordValue('e', 0);
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    TerrainModifiers_Create(id, &position, kind, shape, flags);
    return 0;
}
