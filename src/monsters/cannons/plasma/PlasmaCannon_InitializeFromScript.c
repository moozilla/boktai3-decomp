#include "monsters/cannons/plasma_cannon.h"
#include "player/cutscene/player_cue_sequence.h"
#include "engine/math/scene_vec.h"
#include "engine/engine_globals.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Sprite_Init(void *, void *, s32);
void AnimData_SelectPaletteBank(void *, s32);
void PlasmaCannon_InitCollider(void *);
void PlasmaCannon_TryFireAtPlayer(void);
void PlasmaCannon_UpdateDefeatEffect(void);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
static inline s32 PlasmaCannon_CellIndex(struct SceneVec *v)
{
    s32 x = (s16)v->x >> 8, z = (s16)v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
s32 PlasmaCannon_InitializeFromScript(struct OrangeSpineActor *p, void *parent)
{
    void *resource;
    if (Script_SeekToKeyword('p')) {
        p->initialPosition.x = Script_GetValue();
        p->initialPosition.y = Script_GetValue();
        p->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('a')) {
        p->projectileHitX = Script_GetValue();
        p->projectileHitY = Script_GetValue();
        p->projectileHitZ = Script_GetValue();
        p->unknown136 = Script_GetValue();
        p->projectileSpeed = Script_GetValue();
    } else {
        p->projectileHitX = 30;
        p->projectileHitY = 30;
        p->projectileHitZ = 30;
        p->unknown136 = 6;
        p->projectileSpeed = 300;
    }
    if (Script_SeekToKeyword('l')) {
        s32 health = Script_GetValue();
        p->maxHealth = health;
        p->health = health;
        if ((s16)health < 0)
            p->defeatDelay = 1;
    } else {
        p->maxHealth = 120;
        p->health = 120;
    }
    if (Script_SeekToKeyword('d'))
        p->registerTerrain = Script_GetValue();
    if (Script_SeekToKeyword('i'))
        p->recoveryDelay = Script_GetValue();
    else
        p->recoveryDelay = 60;
    if (Script_SeekToKeyword('h')) {
        s8 *b;
        s32 v = Script_GetValue();
        b = (s8 *)&p->facing;
        *b = v;
        *b = (*b - 1) / 2;
        if ((u8)*b > 3)
            *b = 1;
        if (*b <= 1)
            *b = 0;
        else
            *b = 1;
    } else
        p->facing = 0;
    if (Script_SeekToKeyword('S'))
        p->showTerrainBody = Script_GetValue();
    if (Script_SeekToKeyword('z')) {
        p->bodyReactionFlags = Script_GetValue();
        p->sparkMask = Script_GetValue();
    }
    if (Script_SeekToKeyword('g'))
        p->hudStyle = Script_GetValue();
    else
        p->hudStyle = 2;
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
        p->hudTime = Script_GetValue();
        if (p->hudTime > 99)
            p->hudTime = 99;
        if (p->hudTime <= 0)
            p->hudTime = 1;
    } else
        p->hudTime = 0;
    p->hud = parent;
    AnimData_Load(p->graphics, 0x7e7d);
    Sprite_Init(&p->sprite.display, p->graphics, 1);
    p->sprite.coordinates.position = p->initialPosition;
    AnimData_SelectPaletteBank(p->graphics, 0x1c1);
    resource = ScriptProc_Start(0x922e, 0x5184);
    p->resource = resource;
    if (!p->defeatDelay) {
        p->update = PlasmaCannon_TryFireAtPlayer;
        Anim_SetSequence(&p->sequence, resource, 0xe, p->facing, 0);
    } else {
        p->update = PlasmaCannon_UpdateDefeatEffect;
        p->defeated = 1;
        Anim_SetSequence(&p->sequence, p->resource, 0x10,
                         p->facing, 0);
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)(&p->sprite.display),
                           (struct DisplaySequenceState *)(&p->sequence));
    if (!p->defeatDelay)
        PlasmaCannon_InitCollider(p);
    if (p->registerTerrain && !p->defeatDelay) {
        s32 cell = PlasmaCannon_CellIndex(&p->sprite.coordinates.position);
        TerrainModifiers_RegisterCell(p->terrainRegistration, cell, 0, p->sprite.display.position.y >> 8, 0, 1);
    }
    if (p->showTerrainBody) {
        u8 *body = p->unknownC0;
        SceneBody_Init(body, p->hud, &p->sprite.coordinates.position, 0, 4, p);
        SceneBody_SetSprite(body, &p->sprite.display);
        TerrainOverlay_QueueAddActor(body);
    }
#ifdef UBFIX
    // falls off the end with r0 left over from its last statement; the creator only tests
    // < 0, and this initializer has no failure path here
    return 0;
#endif
}
