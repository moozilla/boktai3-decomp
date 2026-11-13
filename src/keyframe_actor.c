#include "global.h"
#include "player_cue_sequence.h"
#include "ram.h"
#include "engine.h"

// 19 differing instruction runs; first diff at +0x4 (adds r6, r0, #0 vs adds r5, r0, #0)

struct BlobAE {
    u32 words[8];
};



void Widget_InitLinked(u8 *, struct BlobAE *, u32, u32, u32, u32, u32, void *);


s32 Script_GetValue(void);

void sub_08238238(u8 *);


void KeyframeActor_InitKeyframeDisplay(u8 *p)
{
    struct BlobAE *data = ScriptProc_Start(0xcb05, 0xde23);
    typeof(p[0x548]) t1;
    if (data) {
        struct BlobAE *copy;
        u8 *object;
        s32 found;
        u32 zero;
        *((struct BlobAE *)(p + 0x6c)) = *data;
        copy = (struct BlobAE *)(p + 0x6c);
        ResourceHeader_Relocate((struct BlobAE *)(p + 0x6c), data);
        object = p + 0x8c;
        Widget_InitLinked(p + 0x8c, (struct BlobAE *)(p + 0x6c), 0, 0x9000, 2, 0, 60, p + 0x30);
        found = Script_SeekToKeyword(0x64);
        if (found)
            p[0x548] = Script_GetValue();
        else
            p[0x548] = found;
        t1 = p[0x548];
        {
            u8 *q = &p[0x3a4];
            zero = 0;
            *q = t1;
        }
        sub_08238238(p);
        *((u16 *)(p + 0x3a0)) = zero;
        Anim_SetKeyframes(p + 0x8c, (struct BlobAE *)(p + 0x6c), p[0x3a2], 1, zero);
        if (p[0x3a3])
            *((u32 *)(p + 0x94)) |= 4;
        else
            *((u32 *)(p + 0x94)) &= ~4;
    }
}

// confidence=low: script body initialization installs this auxiliary sprite and animation.

struct Vec {
    s16 x, y, z;
};

void KeyframeActor_InitAuxiliarySprite(u8 *arg)
{
    u8 *p = arg;
    void *data = p + 0x198;
    void *sequence;
    struct ContactSwarmSprite *render;
    u32 zero;
    struct DisplaySequenceState *frames;
    AnimData_Load(data, 0xD495);
    sequence = ScriptProc_Start(0x922E, 0xAF44);
    *(void **)(p + 0x410) = sequence;
    render = (void *)(p + 0x14C);
    *(void **)(p + 0x178) = data;
    zero = 0;
    Sprite_Init(render, data, 0x1001);
    {
        u32 value = *(u16 *)(p + 0x30);
        *(s16 *)(p + 0x168) = value;
        {
            u32 y = *(u16 *)(p + 0x32);
            *(s16 *)(p + 0x16A) = y;
        }
        value = *(u16 *)(p + 0x34);
        *(s16 *)(p + 0x16C) = value;
    }
    *(u16 *)(p + 0x4E8) = zero;
    frames = (void *)(p + 0x3FC);
    Anim_SetSequence(frames, sequence, 0, p[0x3A2], p[0x3A3]);
    PlayerCue_StepSequence(render, frames);
}

/* tentative: reads the script position and sets up the body, probe and contact node */

struct Vec16 {
    s16 x, y, z;
};

u8 *TerrainModifiers_FindCellOverride(s32, s32);

u32 SceneBody_Init(void *, u32, void *, u32, u32, void *);

void KeyframeActor_InitKeyframeDisplay(u8 *);

void KeyframeActor_InitAuxiliarySprite(u8 *);

u32 SceneBody_SetKeyframeDisplay(void *, void *);

void CellObject_Init(void *, void *);

void SceneBody_SetMovementProbe(void *, void *, s32, s32);

u32 SceneBody_SetPositionOffset(void *, struct Vec16 *);

u32 SceneActorContacts_RegisterNode(void *, void *, u32);

u32 KeyframeActor_InitBodyFromScript(u8 *p, u32 entry)
{
    s32 found = Script_SeekToKeyword('p');
    struct Vec16 *pos;
    s32 x, z, cell;
    u8 *prop;
    u32 surface, height;
    u8 *body;
    struct Vec16 off;
    u32 zero;
    u32 id;

    if (found) {
        *(s16 *)(p + 0x540) = Script_GetValue();
        *(s16 *)(p + 0x542) = Script_GetValue();
        *(s16 *)(p + 0x544) = Script_GetValue();
    } else {
        *(s16 *)(p + 0x540) = found;
        *(s16 *)(p + 0x542) = found;
        *(s16 *)(p + 0x544) = found;
    }
    pos = (struct Vec16 *)(p + 0x540);
    x = pos->x >> 8;
    z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else {
        u32 row = gTerrainMap->rows[z];
        cell = row + x;
    }
    prop = TerrainModifiers_FindCellOverride(cell, 1);
    if (prop)
        prop += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        prop = map->cells + offset;
    }
    surface = *prop >> 4;
    height = (*prop & 15) << 8;
    switch (surface) {
        case 1:
            height -= (u8)pos->z;
            break;
        case 2:
            height -= (u8)pos->x;
            break;
    }
    {
        s16 *h = (s16 *)(p + 0x542);
        zero = 0;
        *h = height;
    }
    body = p + 0x24;
    SceneBody_Init(body, entry, p + 0x540, 0, p[0x18], p);
    KeyframeActor_InitKeyframeDisplay(p);
    KeyframeActor_InitAuxiliarySprite(p);
    p[0x419] = zero;
    SceneBody_SetKeyframeDisplay(body, p + 0x8c);
    CellObject_Init(p + 0x284, p + 0x540);
    SceneBody_SetMovementProbe(body, p + 0x284, 0x1e, 0x1e);
    off.x = 0;
    off.y = 0x12c;
    off.z = 0;
    SceneBody_SetPositionOffset(body, &off);
    id = *(u32 *)(p + 0x24);
    *(u16 *)(p + 0x294) = id;
    SceneActorContacts_RegisterNode(p + 0x2e8, body, 2);
    gTrackedPlayerId[*(u32 *)(p + 0x18)] = *(u32 *)(p + 0x24);
}
