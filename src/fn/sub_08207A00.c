#include "global.h"
#include "vulcan_cannon_worker.h"
#include "engine.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void *Script_GetValueSafe(void);
void Sprite_Init(void *, void *, s32);
void VulcanCannon_UpdateTargeting(void);
void VulcanCannon_UpdateDefeatEffects(void);
void VulcanCannon_InitCollider(void *);
void VulcanCannon_ReadScriptValueList(void *, void *);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(void *, void *, void *, s32, s32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);

s32 VulcanCannon_InitializeFromScript(struct VulcanCannon *p, void *owner)
{
    if (Script_SeekToKeyword('p')) {
        p->initialPosition.x = Script_GetValue();
        p->initialPosition.y = Script_GetValue();
        p->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('a')) {
        p->attack = Script_GetValue();
        p->element = Script_GetValue();
        p->power = Script_GetValue();
        p->speed = Script_GetValue();
    } else {
        p->attack = 30;
        p->element = 30;
        p->power = 30;
        p->speed = 24;
    }
    if (Script_SeekToKeyword('R'))
        p->range = Script_GetValue();
    else
        p->range = 1024;
    if (Script_SeekToKeyword('l')) {
        s32 hp = Script_GetValue();
        p->maxHp = hp;
        p->hp = hp;
        if (((s16)hp) < 0)
            p->inert = 1;
    } else {
        p->maxHp = 120;
        p->hp = 120;
    }
    if (Script_SeekToKeyword('d'))
        p->terrainEnabled = Script_GetValue();
    if (Script_SeekToKeyword('i'))
        p->turnDelay = Script_GetValue();
    else
        p->turnDelay = 60;
    if (Script_SeekToKeyword('h')) {
        p->facing = Script_GetValue();
        p->allowTurn = Script_GetValue();
        {
            u32 facing = (u8)(p->facing = (p->facing - 1) / 2);
            if (facing > 3)
                p->facing = 1;
            else if (facing <= 1)
                p->facing = 0;
            else
                p->facing = 1;
        }
    } else {
        p->facing = 1;
        p->allowTurn = 1;
    }
    if (Script_SeekToKeyword('r'))
        p->delayScript = Script_GetValueSafe();
    if (Script_SeekToKeyword('I'))
        p->burstDelay = Script_GetValue();
    else
        p->burstDelay = 120;
    if (Script_SeekToKeyword('n'))
        p->burstCount = Script_GetValue();
    else
        p->burstCount = 3;
    if (Script_SeekToKeyword('S'))
        p->unknown14c = Script_GetValue();
    if (Script_SeekToKeyword('z')) {
        p->bodyFlags = Script_GetValue();
        p->bodyPower = Script_GetValue();
    }
    if (Script_SeekToKeyword('g'))
        p->hudStyle = Script_GetValue();
    else
        p->hudStyle = 2;
    if (Script_SeekToKeyword('D'))
        p->deathScript = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        p->args[0] = Script_GetValue();
        p->args[1] = Script_GetValue();
        p->args[2] = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        p->defeatId = Script_GetValue();
        p->unknown152 = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        p->hudDuration = Script_GetValue();
        if (p->hudDuration > 99)
            p->hudDuration = 99;
        if (p->hudDuration <= 0)
            p->hudDuration = 1;
    } else
        p->hudDuration = 0;
    p->owner = owner;
    AnimData_Load(p->graphics, 0x7e7d);
    Sprite_Init(&p->display.sprite, p->graphics, 1);
    p->display.spatial.position = p->initialPosition;
    AnimData_SelectPaletteBank(p->graphics, 0x1be);
    p->resource = ScriptProc_Start(0x922e, 0x5184);
    if (!p->inert) {
        p->state = VulcanCannon_UpdateTargeting;
        Anim_SetSequence(&p->sequence, p->resource, 0x11, (u8)p->facing, 0);
    } else {
        p->state = VulcanCannon_UpdateDefeatEffects;
        p->dead = 1;
        Anim_SetSequence(&p->sequence, p->resource, 0x13, (u8)p->facing, 0);
    }
    VulcanCannon_StepAnimation(p);
    if (!p->inert)
        VulcanCannon_InitCollider(p);
    if (p->terrainEnabled && (!p->inert)) {
        struct SceneVec *position = &p->display.spatial.position;
        s32 x = position->x >> 8;
        s32 z = position->z >> 8;
        s32 index;
        if ((((x < 0) || (z < 0)) || (((u32)x) >= gTerrainWidth)) || (((u32)z) >= gTerrainHeight))
            index = 0;
        else
            index = gTerrainMap->rows[z] + x;
        TerrainModifiers_RegisterCell(p->terrain, index, 0, p->display.spatial.position.y >> 8, 0,
                                      1);
    }
    SceneBody_Init(p->body, p->owner, &p->display.spatial.position, 0, 4, p);
    if (p->unknown14c) {
        SceneBody_SetSprite(p->body, &p->display.sprite);
        TerrainOverlay_QueueAddActor(p->body);
    }
    if (p->delayScript)
        VulcanCannon_ReadScriptValueList(p, p->delayScript);
}
