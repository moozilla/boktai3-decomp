#include "monsters/dragon_puppy.h"
#include "player/cutscene/player_cue_sequence.h"
#include "engine/math/scene_vec.h"
#include "engine/engine_globals.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
s32 Script_GetValueSafe(void);
void Sprite_Init(void *, void *, s32);
void AnimData_SelectPaletteBank(void *, s32);
void DragonPuppy_InitCollider(void *);
void DragonPuppy_UpdateAttackTiming(void);
void DragonPuppy_ReadAttackSchedule(void *);
void DragonPuppy_UpdateAttackWindup(void);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
static inline s32 DragonPuppy_CellIndex(struct ContactPosition *v)
{
    s32 x = (s16)v->x >> 8, z = (s16)v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
s32 DragonPuppy_InitializeFromScript(struct DragonPuppy *p, void *parent)
{
    void *resource;
    if (Script_SeekToKeyword('p')) {
        p->initialPosition.x = Script_GetValue();
        p->initialPosition.y = Script_GetValue();
        p->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('a')) {
        p->attackX = Script_GetValue();
        p->attackY = Script_GetValue();
        p->attackZ = Script_GetValue();
        p->unknown136 = Script_GetValue();
    } else {
        p->attackX = 30;
        p->attackY = 30;
        p->attackZ = 30;
        p->unknown136 = 24;
    }
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
    if (Script_SeekToKeyword('d'))
        p->terrainEnabled = Script_GetValue();
    if (Script_SeekToKeyword('i'))
        p->attackInterval = Script_GetValue();
    else
        p->attackInterval = 60;
    if (Script_SeekToKeyword('h')) {
        s32 v = Script_GetValue();
        u8 *b = &p->deathVariant;
        *b = v;
        *b = ((s8)*b - 1) / 2;
        if (*b > 3)
            *b = 1;
    } else
        p->deathVariant = 1;
    if (Script_SeekToKeyword('r'))
        p->attackSchedule = Script_GetValueSafe();
    if (Script_SeekToKeyword('f'))
        p->unknown13A = Script_GetValue();
    else
        p->unknown13A = 600;
    if (Script_SeekToKeyword('S'))
        p->showTerrainBody = Script_GetValue();
    if (Script_SeekToKeyword('z')) {
        p->bodyReactionFlags = Script_GetValue();
        p->sparkMask = Script_GetValue();
    }
    if (Script_SeekToKeyword('g'))
        p->hudValue140 = Script_GetValue();
    else
        p->hudValue140 = 2;
    if (Script_SeekToKeyword('D'))
        p->unknown168 = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        p->unknown158 = Script_GetValue();
        p->unknown15C = Script_GetValue();
        p->unknown160 = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        p->unknown144 = Script_GetValue();
        p->unknown146 = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        p->hudValue148 = Script_GetValue();
        if (p->hudValue148 > 99)
            p->hudValue148 = 99;
        if (p->hudValue148 <= 0)
            p->hudValue148 = 1;
    } else
        p->hudValue148 = 0;
    p->context = parent;
    AnimData_Load(p->palette, 0x7e7d);
    Sprite_Init(&p->sprite, p->palette, 1);
    p->sprite.position = p->initialPosition;
    AnimData_SelectPaletteBank(p->palette, 0x1b9);
    resource = ScriptProc_Start(0x922e, 0x5184);
    p->deathAnimation = resource;
    if (!p->initialDead) {
        p->update = DragonPuppy_UpdateAttackTiming;
        Anim_SetSequence(&p->animation, resource, 3, p->deathVariant, 0);
    } else {
        p->update = DragonPuppy_UpdateAttackWindup;
        Anim_SetSequence(&p->animation, resource, 5, p->deathVariant, 0);
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)(&p->sprite),
                           (struct DisplaySequenceState *)(&p->animation));
    if (!p->initialDead)
        DragonPuppy_InitCollider(p);
    if (p->terrainEnabled && !p->initialDead) {
        s32 cell = DragonPuppy_CellIndex(&p->sprite.position);
        TerrainModifiers_RegisterCell(p->terrain, cell, 0, p->sprite.position.y >> 8, 0, 1);
    }
    if (p->showTerrainBody) {
        u8 *body = p->terrainBody;
        SceneBody_Init(body, p->context, &p->sprite.position, 0, 4, p);
        SceneBody_SetSprite(body, &p->sprite);
        TerrainOverlay_QueueAddActor(body);
    }
    if (p->attackSchedule)
        DragonPuppy_ReadAttackSchedule(p);
#ifdef UBFIX
    // falls off the end with r0 left over from its last statement; the creator only tests
    // < 0, and this initializer has no failure path here
    return 0;
#endif
}
