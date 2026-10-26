#include "engine.h"
struct HazardVec {
    s16 x, y, z, pad;
};
struct HazardPackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct HazardSprite {
    u32 flags;
    u8 pad4[3], priority;
    u8 pad8[0x2C - 8];
};
struct TimedHazard {
    u8 id, entered, state, flags;
    u8 pad4, unknown5;
    u16 timer, delay;
    u8 padA[2];
    struct HazardVec position;
    u16 ground;
    u16 health;
    void *collider, *attack;
    u8 resource[0x1C];
    struct HazardSprite sprite;
    u8 anim[0x10];
    void *previous, *next;
};
struct TimedHazardPool {
    u8 task[0x18];
    void *animation;
};
extern struct TimedHazardPool *gColliderSpritePool;
void *ColliderSpritePool_Create(u32, u32);
void TimedHazard_OnDamageContact();
void TimedHazard_OnAttackContact();
void Collider_SetAttackPowerAndForce(void *, u32, u32);
u32 TimedHazard_LinkInstance(struct TimedHazardPool *, struct TimedHazard *);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
struct Cell {
    u8 height, tile;
    u16 flags;
};
static inline struct Cell *getCell(s32 index)
{
    u8 *dynamic = TerrainModifiers_FindCellOverride(index, 1);
    if (dynamic)
        return (struct Cell *)(dynamic + 4);
    return (struct Cell *)(gTerrainMap->cells + 12 + index * 4);
}
s32 sub_08232420(s32 id, s32 flags, s32 delay, s32 health, struct HazardVec *position)
{
    struct HazardPackedVec size, offset;
    struct TimedHazard *p;
    struct HazardVec *pos;
    void *resource;
    if (!gColliderSpritePool && !ColliderSpritePool_Create(0xC0DB, 0))
        return -1;
    p = Heap_Alloc(sizeof(*p));
    if (!p)
        return -1;
    Memory_Clear(p, sizeof(*p));
    p->id = id;
    p->flags = flags;
    p->delay = delay;
    p->position = *position;
    {
        struct HazardVec *groundPos = &p->position;
        s32 x = p->position.x >> 8, z = groundPos->z >> 8, index;
        struct Cell *cell;
        u32 slope;
        s32 height;
        pos = groundPos;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            index = 0;
        else
            index = gTerrainMap->rows[z] + x;
        cell = getCell(index);
        slope = cell->height >> 4;
        height = (cell->height & 15) << 8;
        switch (slope) {
            case 1:
                height -= (u8)groundPos->z;
                break;
            case 2:
                height -= (u8)groundPos->x;
                break;
        }
        p->ground = height;
    }
    p->health = health;
    if (p->flags & 1) {
        p->collider = Heap_Alloc(84);
        if (!p->collider)
            return -1;
        Memory_Clear(p->collider, 84);
        size.x = 90;
        size.y = 128;
        size.z = 90;
        offset.x = 0;
        offset.y = 128;
        offset.z = 0;
        Collider_Init(p->collider, 0, 0x4001, 0, 16, &size, &offset);
        Collider_SetCallback(p->collider, TimedHazard_OnDamageContact, p);
        Collider_Refresh(p->collider);
        Collider_SetPosition(p->collider, pos, 0);
    } else
        p->collider = 0;
    if (p->flags & 1) {
        u32 zero;
        p->attack = Heap_Alloc(84);
        if (!p->attack)
            return -1;
        Memory_Clear(p->attack, 84);
        zero = 0;
        ((struct HazardVec *)&size)->x = 100;
        ((struct HazardVec *)&size)->y = 120;
        ((struct HazardVec *)&size)->z = 100;
        ((struct HazardVec *)&offset)->x = zero;
        ((struct HazardVec *)&offset)->y = zero;
        ((struct HazardVec *)&offset)->z = zero;
        Collider_Init(p->attack, 0, 0x2001, 0, 16, &size, &offset);
        Collider_SetCallback(p->attack, TimedHazard_OnAttackContact, p);
        Collider_SetAttackPowerAndForce(p->attack, 50, 20);
        Collider_Refresh(p->attack);
        Collider_SetPosition(p->attack, pos, 0);
    } else
        p->attack = 0;
    resource = p->resource;
    if (!AnimData_Load(resource, 0x863A))
        return -1;
    Sprite_Init(&p->sprite, resource, 1);
    p->sprite.priority = 2;
    Anim_SetSequence(p->anim, gColliderSpritePool->animation, 1, 0, 0);
    AnimData_SelectPaletteBank(resource, p->id + 0x153);
    if (p->delay == 0) {
        p->state = 1;
        p->timer = 0;
        p->entered = 1;
    } else {
        p->state = 0;
        p->timer = 0;
        p->entered = 1;
    }
    p->unknown5 = 0;
    p->previous = 0;
    p->next = 0;
    return TimedHazard_LinkInstance(gColliderSpritePool, p);
}
int Script_GetKeywordValue(int, int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
/* Script command handler that follows sub_08232420 inside its ROM symbol (no label of its own). */
// Provisional name: TimedHazard_CreateFromScript (hidden function, confidence low)
s32 sub_0823269C(void)
{
    struct HazardPackedVec position;
    s32 id, flags, delay, health;
    id = Script_GetKeywordValue('t', 0);
    flags = Script_GetKeywordValue('f', 0);
    delay = Script_GetKeywordValue('e', 0);
    health = Script_GetKeywordValue('l', 0);
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    return sub_08232420(id, flags, delay, health, (struct HazardVec *)&position);
}
