#include "monsters/poison_hand.h"
#include "player/cutscene/player_cue_sequence.h"
#include "engine/math/scene_vec.h"
#include "engine/engine_globals.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Sprite_Init(void *, void *, s32);
void AnimData_SelectPaletteBank(void *, s32);
void PoisonHand_InitCollider(void *);
u32 PoisonHand_WaitForPlayerApproach(u8 *);
void PoisonHand_UpdateDefeatBurst(void);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
static inline s32 PoisonHand_CellIndex(struct ContactPosition *v)
{
    s32 x = (s16)v->x >> 8, z = (s16)v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
s32 PoisonHand_InitializeFromScript(struct PoisonHand *p, void *parent)
{
    void *resource;
    if (Script_SeekToKeyword('p')) {
        p->initialPosition.x = Script_GetValue();
        p->initialPosition.y = Script_GetValue();
        p->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('r')) {
        s32 r = Script_GetValue();
        p->attackDistanceSquared = r * r;
    } else
        p->attackDistanceSquared = 0x40000;
    if (Script_SeekToKeyword('a')) {
        p->effectX = Script_GetValue();
        p->effectY = Script_GetValue();
        p->effectZ = Script_GetValue();
    } else {
        p->effectX = 0;
        p->effectY = 0;
        p->effectZ = 30;
    }
    if (Script_SeekToKeyword('s'))
        p->spread = Script_GetValue();
    else
        p->spread = 2;
    if (Script_SeekToKeyword('d'))
        p->registerTerrain = Script_GetValue();
    if (Script_SeekToKeyword('l')) {
        s32 health = Script_GetValue();
        p->maxHealth = health;
        p->health = health;
        if ((s16)health < 0)
            p->initialDead = 1;
    } else {
        p->maxHealth = 120;
        p->health = 120;
    }
    if (Script_SeekToKeyword('S'))
        p->showTerrainBody = Script_GetValue();
    if (Script_SeekToKeyword('z')) {
        p->bodyReactionFlags = Script_GetValue();
        p->sparkMask = Script_GetValue();
    }
    if (Script_SeekToKeyword('g'))
        p->hudState = Script_GetValue();
    else
        p->hudState = 2;
    if (Script_SeekToKeyword('D'))
        p->unknown164 = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        p->unknown158 = Script_GetValue();
        p->unknown15C = Script_GetValue();
        p->unknown160 = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        p->unknown148 = Script_GetValue();
        p->unknown14A = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        p->hudTimer = Script_GetValue();
        if (p->hudTimer > 99)
            p->hudTimer = 99;
        if (p->hudTimer <= 0)
            p->hudTimer = 1;
    } else
        p->hudTimer = 0;
    p->hud = parent;
    AnimData_Load(p->graphics, 0x7e7d);
    Sprite_Init(&p->sprite, p->graphics, 1);
    *(struct SceneVec *)&p->sprite.position = p->initialPosition;
    AnimData_SelectPaletteBank(p->graphics, 0x1c5);
    resource = ScriptProc_Start(0x922e, 0x5184);
    p->animationResource = resource;
    if (!p->initialDead) {
        p->update = (void (*)(void))PoisonHand_WaitForPlayerApproach;
        Anim_SetSequence(&p->sequence, resource, 0x17, 0, 0);
    } else {
        p->update = PoisonHand_UpdateDefeatBurst;
        p->defeated = 1;
        Anim_SetSequence(&p->sequence, p->animationResource, 0x19, 0, 0);
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)(&p->sprite),
                           (struct DisplaySequenceState *)(&p->sequence));
    if (!p->initialDead)
        PoisonHand_InitCollider(p);
    if (p->registerTerrain && !p->initialDead) {
        s32 cell = PoisonHand_CellIndex(&p->sprite.position);
        TerrainModifiers_RegisterCell(p->terrainRegistration, cell, 0, p->sprite.position.y >> 8, 0, 1);
    }
    if (p->showTerrainBody) {
        u8 *body = p->terrainBody;
        SceneBody_Init(body, p->hud, &p->sprite.position, 0, 4, p);
        SceneBody_SetSprite(body, &p->sprite);
        TerrainOverlay_QueueAddActor(body);
    }
#ifdef UBFIX
    // falls off the end with r0 left over from its last statement; the creator only tests
    // < 0, and this initializer has no failure path here
    return 0;
#endif
}
