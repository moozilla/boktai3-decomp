#include "global.h"
// confidence=low: parses the spawn script (position, attack, health, timing keywords), loads
// graphics and the first animation frame, and installs the idle callback.
#include "scene_vec.h"
#include "engine_globals.h"
#include "player_cue_sequence.h"
struct PHVec {
    s16 x, y, z, pad;
};
struct FanSeq {
    const volatile u16 *commands;
    u8 flags, count, weight, duration;
    u16 index, unused, speed, elapsed;
};
struct PHTerrain {
    u8 pad[0x24];
    u16 rows[1];
};
void EnemyF426_InitCollider(void *);
void sub_08203CA0(void);
s32 Script_GetValueSafe(void);
void EnemyF426_ReadScriptValueList(void *);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
#define H(o) (*(s16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
static inline s32 EnemyF426_CellIndex(struct PHVec *v)
{
    s32 x = (s16)v->x >> 8, z = (s16)v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return ((struct PHTerrain *)gTerrainMap)->rows[z] + x;
}
/* The state callback, sequence, hit flag, facing, defeat flag and resource slot the script setup writes. */
// confidence=low: field names follow FanCannon, whose setup tail this mirrors.
struct EnemyF426Tail {
    u8 pad0[0x108];
    void (*update)(void);
    u8 sequence[0x22];
    s16 hit;
    u8 pad130[2];
    u8 direction;
    u8 pad133[0x11];
    s16 dead;
    u8 pad146[0xe];
    void *resource;
};
s32 EnemyF426_InitializeFromScript(u8 *p, void *parent)
{
    void *resource;
    s32 found;
    if (Script_SeekToKeyword('p')) {
        H(0x1c) = Script_GetValue();
        H(0x1e) = Script_GetValue();
        H(0x20) = Script_GetValue();
    }
    if (Script_SeekToKeyword('a')) {
        H(0x120) = Script_GetValue();
        H(0x122) = Script_GetValue();
        H(0x124) = Script_GetValue();
        H(0x136) = Script_GetValue();
    } else {
        H(0x120) = 30;
        H(0x122) = 30;
        H(0x124) = 30;
        H(0x136) = 24;
    }
    if (Script_SeekToKeyword('l')) {
        s32 health = Script_GetValue();
        H(0x138) = health;
        H(0x126) = health;
        if ((s16)health < 0)
            H(0x144) = 1;
    } else {
        H(0x138) = 120;
        H(0x126) = 120;
    }
    if (Script_SeekToKeyword('d'))
        W(0x150) = Script_GetValue();
    if (Script_SeekToKeyword('i'))
        H(0x134) = Script_GetValue();
    else
        H(0x134) = 60;
    if (Script_SeekToKeyword('h')) {
        s32 v = Script_GetValue();
        s8 *b = (s8 *)(p + 0x132);
        *b = v;
        *b = (*b - 1) / 2;
        if ((u8)*b > 3)
            *b = 1;
    } else
        *(u8 *)(p + 0x132) = 1;
    if (Script_SeekToKeyword('r'))
        W(0x148) = Script_GetValueSafe();
    if (Script_SeekToKeyword('S'))
        H(0x140) = Script_GetValue();
    if (Script_SeekToKeyword('z')) {
        H(0x13a) = Script_GetValue();
        H(0x13c) = Script_GetValue();
    }
    if (Script_SeekToKeyword('g'))
        H(0x13e) = Script_GetValue();
    else
        H(0x13e) = 2;
    if (Script_SeekToKeyword('D'))
        W(0x14c) = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        W(0x168) = Script_GetValue();
        W(0x16c) = Script_GetValue();
        W(0x170) = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        H(0x158) = Script_GetValue();
        H(0x15a) = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        H(0x142) = Script_GetValue();
        if (H(0x142) > 99)
            H(0x142) = 99;
        if (H(0x142) <= 0)
            H(0x142) = 1;
    } else
        H(0x142) = 0;
    *(void **)(p + 0x15c) = parent;
    AnimData_Load(p + 0x50, 0x7e82);
    Sprite_Init(p + 0x24, p + 0x50, 1);
    *(struct SceneVec *)(p + 0x40) = *(struct SceneVec *)(p + 0x1c);
    AnimData_SelectPaletteBank(p + 0x50, 0x225);
    resource = ScriptProc_Start(0x922e, 0xfa45);
    ((struct EnemyF426Tail *)p)->resource = resource;
    if (!((struct EnemyF426Tail *)p)->dead) {
        Anim_SetSequence(p + 0x10c, resource, 0, ((struct EnemyF426Tail *)p)->direction, 0);
    } else {
        ((struct EnemyF426Tail *)p)->hit = 1;
        Anim_SetSequence(p + 0x10c, ((struct EnemyF426Tail *)p)->resource, 1,
                         ((struct EnemyF426Tail *)p)->direction, 0);
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)(p + 0x24),
                           (struct DisplaySequenceState *)(p + 0x10c));
    if (!((struct EnemyF426Tail *)p)->dead)
        EnemyF426_InitCollider(p);
    if (W(0x150) && !((struct EnemyF426Tail *)p)->dead) {
        s32 cell = EnemyF426_CellIndex((struct PHVec *)(p + 0x40));
        TerrainModifiers_RegisterCell(p + 0x174, cell, 0, H(0x42) >> 8, 0, 1);
    }
    if (H(0x140)) {
        u8 *body = p + 0xc0;
        SceneBody_Init(body, *(void **)(p + 0x15c), p + 0x40, 0, 4, p);
        SceneBody_SetSprite(body, p + 0x24);
        TerrainOverlay_QueueAddActor(body);
    }
    if (W(0x148))
        EnemyF426_ReadScriptValueList(p);
    *(void (**)(void))(p + 0x108) = sub_08203CA0;
}
