#include "terrain_modifier.h"
/* TerrainModifiers_NoOp: Body unrecoverable; no callers/callees found; placed where a trivial stub would sit among nearby no-op callbacks, so named speculatively pending re-ex */
#define TerrainModifiers_NoOp sub_08005E10

#define SceneController_ClearGlobal sub_08005E10

void SceneController_ClearGlobal(void)
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
        return value >> 8;
    return -((-value) >> 8);
}

static inline s32 cellIndex(struct TerrainVec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

s32 TerrainModifiers_Create(s32 id, struct TerrainVec *position, s32 kind, s32 tile, s32 flags)
{
    struct TerrainModifier *p;
    if (!gTerrainModifierManager && !TerrainModifierManager_Create(0x2153, 0))
        return 0;
    p = Heap_Alloc(36);
    if (!p)
        return -1;
    Memory_Clear(p, 36);
    p->id = id;
    p->active = 1;
    p->kind = kind;
    p->tile = tile;
    p->cell = cellIndex(position);
    p->height = heightLevel(position->y);
    p->flags = flags;
    TerrainModifiers_RegisterCell(p->registration, p->cell, p->tile, p->height, 255, p->flags);
    TerrainModifiers_AddToList(p);
    return 0;
}

void TerrainModifiers_UnregisterCell(void *);

u32 TerrainModifiers_Destroy(struct TerrainModifier *n)
{
    if (n->active != 0) {
        TerrainModifiers_UnregisterCell(n->registration);
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
            TerrainModifiers_UnregisterCell(n->registration);
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
            TerrainModifiers_RegisterCell(e->registration, e->cell, e->tile, e->height, 0xff,
                                          e->flags);
        }
        e = n;
    }
    return 0;
}

void TerrainModifiers_InitCell(void *, s32, s32, s32, s32, s32);

s32 TerrainModifiers_Change(s32 id, s32 tile, s32 kind, s32 flags, s32 cell, s32 height)
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
            if (tile != -1)
                p->tile = tile;
            if (cell != -1)
                p->cell = cell;
            if (height != -1)
                p->height = height;
            if (flags != -1)
                p->flags = flags;
            TerrainModifiers_InitCell(p->registration, p->cell, p->tile, p->height, 255, p->flags);
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
    void *p = Task_Create(8, 0x1c);
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
    s32 id = Script_GetKeywordValue('n', -1), tile = Script_GetKeywordValue('t', -1),
        kind = Script_GetKeywordValue('f', -1), flags = Script_GetKeywordValue('e', -1);
    struct TerrainScriptVec position;
    s32 cell;
    s32 height;
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
        cell = cellIndex((struct TerrainVec *)&position);
        height = ({
            s32 value = heightLevel(((struct TerrainVec *)&position)->y);
            value;
        });
    } else {
        height = -1;
        cell = height;
    }
    return TerrainModifiers_Change(id, tile, kind, flags, cell, height);
}

u32 TerrainModifiers_CreateFromScript(void)
{
    struct TerrainScriptVec bf;
    u32 a = Script_GetKeywordValue(0x6e, 0);
    u32 b = Script_GetKeywordValue(0x74, 0);
    u32 c = Script_GetKeywordValue(0x66, 0);
    u32 d = Script_GetKeywordValue(0x65, 0);
    if (Script_SeekToKeyword(0x70)) {
        bf.x = Script_GetValue();
        bf.y = Script_GetValue();
        bf.z = Script_GetValue();
    } else {
        bf.x = 0;
        bf.y = 0;
        bf.z = 0;
    }
    TerrainModifiers_Create(a, (struct TerrainVec *)&bf, c, b, d);
    return 0;
}
