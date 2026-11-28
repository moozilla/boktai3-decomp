#include "global.h"
#include "player/player.h"
#include "player/cutscene/player_cue_sequence.h"
#include "engine/ram.h"
#include "engine/engine.h"

struct KeyframeResource {
    u32 words[8];
};

void AnimSprite_InitLinked(void *, void *, u32, u32, u32, u32, u32, void *);

s32 Script_GetValue(void);

void Player_UpdateActionFacingSide(struct Player *);

void KeyframeActor_InitKeyframeDisplay(struct Player *p)
{
    struct KeyframeResource *data = ScriptProc_Start(0xcb05, 0xde23);

    if (data) {
        *(struct KeyframeResource *)p->animationData = *data;
        ResourceHeader_Relocate(p->animationData, data);
        AnimSprite_InitLinked(&p->animation, p->animationData, 0, 0x9000, 2, 0, 60, &p->position);
        if (Script_SeekToKeyword(0x64))
            p->savedLinkFacing = Script_GetValue();
        else
            p->savedLinkFacing = 0;
        p->facing = p->savedLinkFacing;
        Player_UpdateActionFacingSide(p);
        p->animationIndex = 0;
        Anim_SetKeyframes(&p->animation, p->animationData, p->heading, 1, 0);
        if (p->side)
            p->animation.flags |= 4;
        else
            p->animation.flags &= ~4;
    }
}

// confidence=low: script body initialization installs this auxiliary sprite and animation.

struct KeyframeAuxiliaryView {
    u8 unknown0[0x30];
    struct SceneVec position;
    u8 unknown38[0x114];
    struct ContactSwarmSprite sprite; /* 0x14C */
    void *spriteData;                 /* 0x178 */
    u8 unknown17C[0x1C];
    u8 spriteBuffer[0x264];               /* 0x198 */
    struct DisplaySequenceState sequence; /* 0x3FC */
    u8 unknown40C[4];
    void *sequenceData; /* 0x410 */
    u8 unknown414[0xD4];
    u16 formCueTimer; /* 0x4E8 */
};

void KeyframeActor_InitAuxiliarySprite(struct Player *p)
{
    struct KeyframeAuxiliaryView *view = (struct KeyframeAuxiliaryView *)p;
    u8 *data = view->spriteBuffer;
    void *sequence;
    struct ContactSwarmSprite *sprite;
    u32 zero;
    struct DisplaySequenceState *frames;

    AnimData_Load(data, 0xD495);
    sequence = ScriptProc_Start(0x922E, 0xAF44);
    view->sequenceData = sequence;
    sprite = &view->sprite;
    view->spriteData = data;
    zero = 0;
    Sprite_Init(sprite, data, 0x1001);
    view->sprite.position.x = view->position.x;
    view->sprite.position.y = view->position.y;
    view->sprite.position.z = view->position.z;
    view->formCueTimer = zero;
    frames = &view->sequence;
    Anim_SetSequence(frames, sequence, 0, p->heading, p->side);
    PlayerCue_StepSequence(sprite, frames);
}

/* tentative: reads the script position and sets up the body, probe and contact node */

struct Vec16 {
    s16 x, y, z;
};

u8 *TerrainModifiers_FindCellOverride(s32, s32);

u32 SceneBody_Init(void *, u32, void *, u32, u32, void *);

void KeyframeActor_InitKeyframeDisplay(struct Player *);

void KeyframeActor_InitAuxiliarySprite(struct Player *);

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
    KeyframeActor_InitKeyframeDisplay((struct Player *)p);
    KeyframeActor_InitAuxiliarySprite((struct Player *)p);
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
