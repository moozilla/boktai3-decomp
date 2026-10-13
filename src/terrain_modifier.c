#include "terrain_modifier.h"

#define SceneController_ClearGlobal sub_08005E10

void SceneController_ClearGlobal(void)
{
    gUnk_02000028 = 0;
}

u32 sub_08005E1C(struct TerrainModifier *p)
{
    struct TerrainModifier *t;
    p->previous = 0;
    t = gUnk_02000028->head;
    p->next = t;
    if (t) {
        t->previous = p;
    }
    gUnk_02000028->head = p;
    return 0;
}

u32 sub_08005E40(struct TerrainModifier *p)
{
    struct TerrainModifier *prev = p->previous;
    struct TerrainModifier *next = p->next;
    if (prev) {
        prev->next = next;
    } else {
        gUnk_02000028->head = next;
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
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}

s32 TerrainModifiers_Create(s32 id, struct TerrainVec *position, s32 kind, s32 tile, s32 flags)
{
    struct TerrainModifier *p;
    if (!gUnk_02000028 && !sub_08006144(0x2153, 0))
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
    sub_0821D698(p->registration, p->cell, p->tile, p->height, 255, p->flags);
    sub_08005E1C(p);
    return 0;
}

void sub_0821D6D0(void *);

u32 sub_08005F40(struct TerrainModifier *n)
{
    if (n->active != 0) {
        sub_0821D6D0(n->registration);
        n->active = 0;
    }
    if (gUnk_02000028 != 0)
        sub_08005E40(n);
    Heap_Free(n);
    return 0;
}

s32 sub_08005F78(void)
{
    struct TerrainModifier *n;
    if (gUnk_02000028 == 0)
        return -1;
    n = gUnk_02000028->head;
    while (n != 0) {
        struct TerrainModifier *next = n->next;
        sub_08005F40(n);
        n = next;
    }
    return 0;
}

s32 sub_08005FA8(u16 id)
{
    struct TerrainModifier *n;
    if (gUnk_02000028 == 0)
        return -1;
    n = gUnk_02000028->head;
    while (n != 0) {
        struct TerrainModifier *next = n->next;
        if (n->id == id)
            sub_08005F40(n);
        n = next;
    }
    return 0;
}

s32 sub_08005FE4(u16 id)
{
    struct TerrainModifier *n;
    if (gUnk_02000028 == 0)
        return -1;
    n = gUnk_02000028->head;
    while (n != 0) {
        struct TerrainModifier *next = n->next;
        if (n->id == id && n->active != 0) {
            n->active = 0;
            sub_0821D6D0(n->registration);
        }
        n = next;
    }
    return 0;
}

s32 sub_0800602C(u32 id)
{
    struct TerrainModifier *e;
    struct TerrainModifier *n;
    u16 k = id;
    if (gUnk_02000028 == 0)
        return -1;
    e = gUnk_02000028->head;
    while (e != 0) {
        n = e->next;
        if (e->id == k && e->active == 0) {
            e->active = 1;
            sub_0821D698(e->registration, e->cell, e->tile, e->height, 0xff, e->flags);
        }
        e = n;
    }
    return 0;
}

void sub_0821D630(void *, s32, s32, s32, s32, s32);

s32 TerrainModifiers_Change(s32 id, s32 tile, s32 kind, s32 flags, s32 cell, s32 height)
{
    struct TerrainModifier *p;
    if (!gUnk_02000028)
        return -1;
    p = gUnk_02000028->head;
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
            sub_0821D630(p->registration, p->cell, p->tile, p->height, 255, p->flags);
        }
        p = next;
    }
    return 0;
}

u32 sub_0800611C(void)
{
    return 0;
}

u32 sub_08006120(void)
{
    sub_08005F78();
    gUnk_02000028 = 0;
    return 0;
}

s32 sub_08006134(struct TerrainModifierManager *p, s32 unused)
{
    gUnk_02000028 = p;
    p->head = 0;
    return 0;
}

void *sub_08006144(s32 a, s32 unused)
{
    void *p = Task_Create(8, 0x1c);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0800611C, sub_08006120);
        if (sub_08006134(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

u32 sub_08006184(void)
{
    return sub_08005FA8((u16)Script_GetKeywordValue(0x6e, 0));
}

u32 sub_0800619C(void)
{
    return sub_08005FE4((u16)Script_GetKeywordValue(0x6e, 0));
}

u32 sub_080061B4(void)
{
    return sub_0800602C((u16)Script_GetKeywordValue(0x6e, 0));
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

u32 sub_080062C4(void)
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
