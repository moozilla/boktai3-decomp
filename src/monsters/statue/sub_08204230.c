#include "monsters/statue.h"
// confidence=low: parses the spawn script (position, attack, health, timing keywords), loads
// graphics and the first animation frame, and installs the idle callback.
#include "engine/math/scene_vec.h"
#include "engine/engine_globals.h"
#include "player/cutscene/player_cue_sequence.h"
void Statue_InitCollider(void *);
void sub_08203CA0(void);
s32 Script_GetValueSafe(void);
void Statue_ReadScriptValueList(void *);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
static inline s32 EnemyF426_CellIndex(struct ContactPosition *v)
{
    s32 x = (s16)v->x >> 8, z = (s16)v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
s32 Statue_InitializeFromScript(struct Statue *p, void *parent)
{
    void *resource;
    s32 found;
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
        p->maxHp = health;
        p->hp = health;
        if ((s16)health < 0)
            p->initialDead = 1;
    } else {
        p->maxHp = 120;
        p->hp = 120;
    }
    if (Script_SeekToKeyword('d'))
        p->terrainEnabled = Script_GetValue();
    if (Script_SeekToKeyword('i'))
        p->attackInterval = Script_GetValue();
    else
        p->attackInterval = 60;
    if (Script_SeekToKeyword('h')) {
        s32 v = Script_GetValue();
        s8 *b = (s8 *)&p->facing;
        *b = v;
        *b = (*b - 1) / 2;
        if ((u8)*b > 3)
            *b = 1;
    } else
        p->facing = 1;
    if (Script_SeekToKeyword('r'))
        p->scriptValues = Script_GetValueSafe();
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
        p->unknown14C = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        p->unknown168 = Script_GetValue();
        p->unknown16C = Script_GetValue();
        p->unknown170 = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        p->unknown158 = Script_GetValue();
        p->unknown15A = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        p->hudTime = Script_GetValue();
        if (p->hudTime > 99)
            p->hudTime = 99;
        if (p->hudTime <= 0)
            p->hudTime = 1;
    } else
        p->hudTime = 0;
    p->owner = parent;
    AnimData_Load(&p->graphics, 0x7e82);
    Sprite_Init(&p->sprite, &p->graphics, 1);
    p->sprite.position = p->initialPosition;
    AnimData_SelectPaletteBank(&p->graphics, 0x225);
    resource = ScriptProc_Start(0x922e, 0xfa45);
    p->resource = resource;
    if (!p->initialDead) {
        Anim_SetSequence(&p->sequence, resource, 0, p->facing, 0);
    } else {
        p->dead = 1;
        Anim_SetSequence(&p->sequence, p->resource, 1,
                         p->facing, 0);
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)(&p->sprite),
                           (struct DisplaySequenceState *)(&p->sequence));
    if (!p->initialDead)
        Statue_InitCollider(p);
    if (p->terrainEnabled && !p->initialDead) {
        s32 cell = EnemyF426_CellIndex(&p->sprite.position);
        TerrainModifiers_RegisterCell(p->terrain, cell, 0, p->sprite.position.y >> 8, 0, 1);
    }
    if (p->showTerrainBody) {
        u8 *body = p->terrainBody;
        SceneBody_Init(body, p->owner, &p->sprite.position, 0, 4, p);
        SceneBody_SetSprite(body, &p->sprite);
        TerrainOverlay_QueueAddActor(body);
    }
    if (p->scriptValues)
        Statue_ReadScriptValueList(p);
    p->update = sub_08203CA0;
#ifdef UBFIX
    // falls off the end with r0 left over from its last statement; the creator only tests
    // < 0, and this initializer has no failure path here
    return 0;
#endif
}
