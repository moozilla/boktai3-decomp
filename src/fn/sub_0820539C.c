#include "global.h"
#include "scene_vec.h"
#include "display_animation.h"
#include "display_sequence_inline.h"
#include "engine_globals.h"
union EnemyD459Sprite {
    struct DisplaySequenceSprite display;
    struct {
        u8 unknown0[0x1C];
        struct SceneVec position;
    } coordinates;
};
struct EnemyD459Actor {
    u8 unknown0[0x18];
    s32 timer;
    struct SceneVec initialPosition;
    union EnemyD459Sprite sprite;
    u8 graphics[0x18];
    u8 unknown68[0x58];
    u8 body[0x48];
    void *update;
    struct DisplaySequenceState sequence;
    u8 unknown11C[0x4];
    s16 attackA;
    s16 attackB;
    s16 attackC;
    s16 health;
    u8 unknown128[0x6];
    s16 dead;
    u8 unknown130[0x2];
    s8 facing;
    u8 unknown133[0x1];
    s16 interval;
    s16 attackD;
    s16 maxHealth;
    s16 rangeA;
    s16 rangeB;
    s16 hudStyle;
    s16 bodyEnabled;
    s16 hudTime;
    s16 initialDead;
    u8 unknown146[0x2];
    s16 unknown148;
    s16 unknown14A;
    u32 scriptList;
    u32 valueD;
    u32 terrainEnabled;
    void *resource;
    void *parent;
    u8 terrain[0x10];
    u32 valueG0;
    u32 valueG1;
    u32 valueG2;
};
struct PHTerrain {
    u8 pad[0x24];
    u16 rows[1];
};
struct SpawnOffset {
    s32 x, z;
};
struct SpawnOffsets {
    struct SpawnOffset e[4];
};
extern const struct SpawnOffsets gUnk_0824FE5C;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
s32 Script_GetValueSafe(void);
void Sprite_Init(void *, void *, s32);
void AnimData_SelectPaletteBank(void *, s32);
void EnemyD459_InitCollider(u8 *);
void EnemyD459_ReadScriptValueList(u8 *);
s32 EnemyD459_UpdateAttackTiming(void);
s32 EnemyD459_UpdateDeath(u8 *);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
static inline s32 EnemyD459_CellIndex(struct SceneVec *v)
{
    s32 x = (s16)v->x >> 8, z = (s16)v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return ((struct PHTerrain *)gTerrainMap)->rows[z] + x;
}
static inline void EnemyD459_GetFlags(u32 **out, struct DisplaySequenceSprite *sprite)
{
    *out = &sprite->flags;
}
static inline u32 EnemyD459_StepSequence(u32 *flipFlags, struct EnemyD459Actor *actor,
                                         struct DisplaySequenceSprite *owner,
                                         struct DisplaySequenceState *state)
{
    const u16 *command;
    u32 ended;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    owner->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            actor->sprite.display.flags |= 4;
        else
            actor->sprite.display.flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            *flipFlags |= 8;
        else
            *flipFlags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
                ended = 1;
            } else {
                state->index--;
                ended = 0;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
                ended = 1;
            } else {
                ended = 0;
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    } else {
        ended = 0;
    }
    return ended;
}

inline static u32 Arg1(u32 k_, u8 *p)
{
    return p + k_;
}

s32 EnemyD459_InitializeFromScript(struct EnemyD459Actor *p, void *parent)
{
    struct SpawnOffsets offsets = gUnk_0824FE5C;
    void *resource;
    void **slot;
    void *sprite;
    s16 *position;
    s16 *zp;
    s32 dead;
    if (Script_SeekToKeyword('p')) {
        p->initialPosition.x = Script_GetValue();
        p->initialPosition.y = Script_GetValue();
        p->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('a')) {
        p->attackA = Script_GetValue();
        p->attackB = Script_GetValue();
        p->attackC = Script_GetValue();
        p->attackD = Script_GetValue();
    } else {
        p->attackA = 30;
        p->attackB = 30;
        p->attackC = 30;
        p->attackD = 24;
    }
    if (Script_SeekToKeyword('l')) {
        s32 health = Script_GetValue();
        p->maxHealth = health;
        p->health = health;
        if (((s16)health) < 0)
            p->initialDead = 1;
    } else {
        p->maxHealth = 120;
        p->health = 120;
    }
    if (Script_SeekToKeyword('d'))
        p->terrainEnabled = Script_GetValue();
    if (Script_SeekToKeyword('i'))
        p->interval = Script_GetValue();
    else
        p->interval = 60;
    if (Script_SeekToKeyword('h')) {
        s32 v = Script_GetValue();
        s8 *h = &(p->facing);
        *h = v;
        *h = ((*h) - 1) / 2;
        if (((u8)(*h)) > 3)
            *h = 1;
    } else
        p->facing = 1;
    if (Script_SeekToKeyword('r'))
        p->scriptList = Script_GetValueSafe();
    if (Script_SeekToKeyword('S'))
        p->bodyEnabled = Script_GetValue();
    if (Script_SeekToKeyword('z')) {
        p->rangeA = Script_GetValue();
        p->rangeB = Script_GetValue();
    }
    if (Script_SeekToKeyword('g'))
        p->hudStyle = Script_GetValue();
    else
        p->hudStyle = 2;
    if (Script_SeekToKeyword('D'))
        p->valueD = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        p->valueG0 = Script_GetValue();
        p->valueG1 = Script_GetValue();
        p->valueG2 = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        p->unknown148 = Script_GetValue();
        p->unknown14A = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        p->hudTime = Script_GetValue();
        if ((p->hudTime) > 99)
            p->hudTime = 99;
        if ((p->hudTime) <= 0)
            p->hudTime = 1;
    } else
        p->hudTime = 0;
    p->parent = parent;
    {
        u8 *graphics = &p->graphics;
        void *earlySprite;
        s16 *x;
        s16 *z;
        s8 *h;
        AnimData_Load(graphics, 0x7e82);
        earlySprite = &p->sprite;
        Sprite_Init(earlySprite, graphics, 1);
        x = &(p->sprite.coordinates.position.x);
        p->sprite.coordinates.position = p->initialPosition;
        h = &(p->facing);
        *x += offsets.e[*h].x << 2;
        z = &(p->sprite.coordinates.position.z);
        *z += offsets.e[*h].z << 2;
        {
            s32 facing = *h;
            sprite = earlySprite;
            position = x;
            zp = z;
            if (facing <= 1)
                *zp -= 0x50;
            else
                *position -= 0x50;
        }
        AnimData_SelectPaletteBank(&p->graphics, 0x227);
    }
    resource = ScriptProc_Start(0x922e, 0xfa45);
    slot = (void **)(&p->resource);
    *slot = resource;
    dead = p->initialDead;
    if (dead == 0) {
        *((void **)(&p->update)) = EnemyD459_UpdateAttackTiming;
        Anim_SetSequence(&p->sequence, resource, 2, (u8)(p->facing), dead);
    } else {
        *((void **)(&p->update)) = EnemyD459_UpdateDeath;
        {
            s16 *d = &(p->dead);
            s32 zero = 0;
            *d = 1;
            Anim_SetSequence(&p->sequence, *slot, 4, 0, zero);
        }
        *position = (p->initialPosition.x) + 0x40;
        *zp = (p->initialPosition.z) + 0x40;
        *position -= 0x60;
        *zp -= 0x60;
    }
    {
        u32 *flipFlags;
        EnemyD459_GetFlags(&flipFlags, sprite);
        EnemyD459_StepSequence(flipFlags, p, sprite, (struct DisplaySequenceState *)(&p->sequence));
    }
    if ((*((s16 *)&p->initialDead)) == 0)
        EnemyD459_InitCollider(p);
    if (p->terrainEnabled && p->initialDead == 0) {
        s32 cell = EnemyD459_CellIndex((struct SceneVec *)position);
        TerrainModifiers_RegisterCell(&p->terrain, cell, 0, (p->sprite.coordinates.position.y) >> 8,
                                      0, 1);
    }
    if (p->bodyEnabled) {
        u8 *body = &p->body;
        SceneBody_Init(body, p->parent, position, 0, 4, p);
        SceneBody_SetSprite(body, sprite);
        TerrainOverlay_QueueAddActor(body);
    }
    if (p->scriptList)
        EnemyD459_ReadScriptValueList(p);
    /* The initializer has no explicit return assignment on this path. */
}
