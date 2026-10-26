#include "global.h"
#include "engine.h"
#include "scene_vec.h"
struct SolarBombOwner {
    u8 pad0[0x18];
    s32 player;
    u8 pad1C[0x41E - 0x1C];
    u16 power;
    u8 pad420[0x4EF - 0x420];
    u8 activeCount;
};
struct SolarBomb {
    u8 task[0x18];
    struct SolarBombOwner *owner;
    u8 pad1C[0x2C - 0x1C];
    u16 frame;
    u8 pad2E[0x38 - 0x2E];
    struct SceneVec position;
    u8 pad40[0x64 - 0x40];
    struct SceneVec source;
    u8 pad6C[0x1D0 - 0x6C];
    u16 primed;
    u16 threshold;
    u16 timer;
    u16 linger;
    u8 pad1D8[2];
    u16 age;
};
struct LinkLevels {
    u8 pad0[0x5C];
    u8 levels[4];
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
extern struct LinkLevels *gUnk_020004B4;
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void SolarBomb_SetState(struct SolarBomb *, void (*)(struct SolarBomb *));
void MovingEffect_AdvanceLifetime(struct SolarBomb *);
s32 Div(s32, s32);
void ExplosionEffect_CreateConfigured(void *, u32, s32, u32, u32, u32, u32, u32);
void SceneStimulus_AddTagged(s32, s32, s32, s32, s32, u32);
struct Cell {
    u8 height, tile;
    u16 flags;
};
static inline s32 cellIndex(struct SceneVec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline struct Cell *getCell(s32 index)
{
    u8 *dynamic = TerrainModifiers_FindCellOverride(index, 1);
    if (dynamic)
        return (struct Cell *)(dynamic + 4);
    return (struct Cell *)(gTerrainMap->cells + 12 + index * 4);
}
static inline u8 LinkLevel(s32 player)
{
    if (!gUnk_020004B4 || player < 0)
        return 0;
    return gUnk_020004B4->levels[player];
}
void sub_08233CAC(struct SolarBomb *p)
{
    s32 level;
    u16 cellFlags;
    if (gEngineFlags & 0x800)
        level = LinkLevel(p->owner->player);
    else
        level = gGameState->unk876;
    if (p->timer) {
        if (p->primed)
            p->timer--;
        else {
            if (level > 0)
                p->timer--;
            if (p->linger)
                p->linger--;
        }
    }
    cellFlags = getCell(cellIndex(&p->source))->flags;
    if (cellFlags & 0x40) {
        SolarBomb_SetState(p, MovingEffect_AdvanceLifetime);
    } else {
        struct SceneVec *pos = &p->source;
        struct Cell *cell = getCell(cellIndex(pos));
        u32 slope = cell->height >> 4;
        s32 height = (cell->height & 15) << 8;
        switch (slope) {
            case 1:
                height -= (u8)pos->z;
                break;
            case 2:
                height -= (u8)pos->x;
                break;
        }
        if (height > p->source.y)
            p->timer = 0;
        else if (height < p->source.y) {
            s32 y = (u16)p->source.y - 30;
            p->source.y = y;
            if ((s16)y < height)
                p->source.y = height;
            p->position = p->source;
        }
    }
    if (p->timer < p->threshold)
        p->frame = (p->age & 3) + 0x14;
    else if (p->timer < p->threshold * 2)
        p->frame = (p->age & 3) + 0x10;
    else
        p->frame = (p->age & 3) + 0xC;
    p->age++;
    if (p->timer == 0) {
        s32 damage;
        s32 i;
        if (p->primed)
            damage = 0x7D;
        else if (gEngineFlags & 0x800)
            damage = p->owner->power * 3 + level * 10;
        else
            damage = Div(p->owner->power, 10) * level;
        for (i = 0; i < 5; i++) {
            struct PackedVec pos;
            pos = *(struct PackedVec *)&p->source;
            pos.y += 0x80;
            pos.x += 0x40;
            pos.z += 0x40;
            switch (i) {
                case 1:
                    pos.x += 0x60;
                    break;
                case 2:
                    pos.z += 0x60;
                    break;
                case 3:
                    pos.z -= 0x60;
                    break;
                case 4:
                    pos.x -= 0x60;
                    break;
            }
            ExplosionEffect_CreateConfigured(&pos, 0, damage, 0x32, 0x241004, 0x1E, 0, i * 4);
        }
        SceneStimulus_AddTagged(p->source.x, p->source.z, 0x400, 0, 1, 0xB546);
        p->owner->activeCount--;
        Task_Kill(p);
    } else if (p->linger == 0) {
        p->owner->activeCount--;
        Task_Kill(p);
    }
}
