#include "monsters/poison_hand.h"
#include "player/cutscene/player_cue_sequence.h"
#include "engine/math/scene_vec.h"
#include "engine/engine_globals.h"
#include "scene/terrain/terrain_cell_probe.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Sprite_Init(void *, void *, s32);
void AnimData_SelectPaletteBank(void *, s32);
void PoisonHand_InitCollider(struct PoisonHand *);
u32 PoisonHand_WaitForPlayerApproach(u8 *);
void PoisonHand_UpdateDefeatBurst(void);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
s32 PoisonHand_InitializeFromScript(struct PoisonHand *hand, void *parent)
{
    void *resource;
    if (Script_SeekToKeyword('p')) {
        hand->initialPosition.x = Script_GetValue();
        hand->initialPosition.y = Script_GetValue();
        hand->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('r')) {
        s32 r = Script_GetValue();
        hand->attackDistanceSquared = r * r;
    } else
        hand->attackDistanceSquared = 0x40000;
    if (Script_SeekToKeyword('a')) {
        hand->effectX = Script_GetValue();
        hand->effectY = Script_GetValue();
        hand->effectZ = Script_GetValue();
    } else {
        hand->effectX = 0;
        hand->effectY = 0;
        hand->effectZ = 30;
    }
    if (Script_SeekToKeyword('s'))
        hand->spread = Script_GetValue();
    else
        hand->spread = 2;
    if (Script_SeekToKeyword('d'))
        hand->registerTerrain = Script_GetValue();
    if (Script_SeekToKeyword('l')) {
        s32 health = Script_GetValue();
        hand->maxHealth = health;
        hand->health = health;
        if ((s16)health < 0)
            hand->initialDead = 1;
    } else {
        hand->maxHealth = 120;
        hand->health = 120;
    }
    if (Script_SeekToKeyword('S'))
        hand->showTerrainBody = Script_GetValue();
    if (Script_SeekToKeyword('z')) {
        hand->bodyReactionFlags = Script_GetValue();
        hand->sparkMask = Script_GetValue();
    }
    if (Script_SeekToKeyword('g'))
        hand->hudState = Script_GetValue();
    else
        hand->hudState = 2;
    if (Script_SeekToKeyword('D'))
        hand->unknown164 = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        hand->unknown158 = Script_GetValue();
        hand->unknown15C = Script_GetValue();
        hand->unknown160 = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        hand->unknown148 = Script_GetValue();
        hand->unknown14A = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        hand->hudTimer = Script_GetValue();
        if (hand->hudTimer > 99)
            hand->hudTimer = 99;
        if (hand->hudTimer <= 0)
            hand->hudTimer = 1;
    } else
        hand->hudTimer = 0;
    hand->hud = parent;
    AnimData_Load(&hand->graphics, 0x7e7d);
    Sprite_Init(&hand->sprite, &hand->graphics, 1);
    *(struct SceneVec *)&hand->sprite.position = hand->initialPosition;
    AnimData_SelectPaletteBank(&hand->graphics, 0x1c5);
    resource = ScriptProc_Start(0x922e, 0x5184);
    hand->animationResource = resource;
    if (!hand->initialDead) {
        hand->update = (void (*)(void))PoisonHand_WaitForPlayerApproach;
        Anim_SetSequence(&hand->sequence, resource, 0x17, 0, 0);
    } else {
        hand->update = PoisonHand_UpdateDefeatBurst;
        hand->defeated = 1;
        Anim_SetSequence(&hand->sequence, hand->animationResource, 0x19, 0, 0);
    }
    PlayerCue_StepSequence(&hand->sprite, &hand->sequence);
    if (!hand->initialDead)
        PoisonHand_InitCollider(hand);
    if (hand->registerTerrain && !hand->initialDead) {
        TerrainModifiers_RegisterCell(hand->terrainRegistration,
                                      cellIndexAt((struct Pos16 *)&hand->sprite.position), 0,
                                      hand->sprite.position.y >> 8, 0, 1);
    }
    if (hand->showTerrainBody) {
        SceneBody_Init(hand->terrainBody, hand->hud, &hand->sprite.position, 0, 4, hand);
        SceneBody_SetSprite(hand->terrainBody, &hand->sprite);
        TerrainOverlay_QueueAddActor(hand->terrainBody);
    }
#ifdef UBFIX
    // falls off the end with r0 left over from its last statement; the creator only tests
    // < 0, and this initializer has no failure path here
    return 0;
#endif
}
