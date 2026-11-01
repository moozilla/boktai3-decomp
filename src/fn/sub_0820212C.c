#include "global.h"
#include "scene_vec.h"
#include "engine_globals.h"
#include "player_cue_sequence.h"
#include "terrain_cell_probe.h"
struct ScriptActorSequence {
    const volatile u16 *commands;
    u8 flags, count, weight, duration;
    u16 index, unused, speed, elapsed;
};
struct ScriptActor {
    u8 unknown0[0x1c];
    struct SceneVec origin;
    u8 sprite[0x1c];
    struct SceneVec position;
    u8 unknown48[8], animation[0x1c], passiveCollider[0x54], bodyCollider[0x54], body[0x48];
    void (*action)(void);
    struct ScriptActorSequence playback;
    s32 attackDistance;
    s16 attackA, attackB, attackC;
    u8 unknown17A[2];
    s16 health;
    u8 unknown17E[2];
    s16 damage;
    u16 unknown182, timer, dead, unknown188, active, flash, duration;
    s16 enabled, delayA, delayB, phase, maximumHealth;
    u16 damageType, damageFlags;
    s16 hudMode, overlay, hudValue, optionA, optionB, initialDead;
    u8 unknown1AA[2];
    void *parent;
    u32 optionD, previousHit, currentHit, optionG0, optionG1, optionG2, terrain;
    void *sequence;
    u8 cell[0x10];
};
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_08201784(void *);
u32 ScriptCombatActor_TryProjectileAttack(u8 *);
void ScriptCombatActor_UpdateDefeatEffect(void);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
/* PlayerCue_StepSequence, except that the first flip writes the actor's own sprite. */
static inline void ScriptActor_StepAnimation(struct ScriptActor *p,
                                             struct ContactSwarmSprite *sprite,
                                             struct DisplaySequenceState *state)
{
    const u16 *command;
    {
        u32 offset = state->index << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            ((struct ContactSwarmSprite *)p->sprite)->flags |= 4;
        else
            ((struct ContactSwarmSprite *)p->sprite)->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
            } else {
                state->index--;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    }
}

#define ScriptCombatActor_InitializeFromScript sub_0820212C
s32 ScriptCombatActor_InitializeFromScript(struct ScriptActor *p, void *parent)
{
    void *resource;
    void *sprite;
    void *earlySprite;
    void **slot;
    s32 found;
    s32 dead;
    if (Script_SeekToKeyword('p')) {
        p->origin.x = Script_GetValue();
        p->origin.y = Script_GetValue();
        p->origin.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('r')) {
        s32 r = Script_GetValue();
        p->attackDistance = r * r;
    } else
        p->attackDistance = 0x64000;
    found = Script_SeekToKeyword('a');
    if (found) {
        p->attackA = Script_GetValue();
        p->attackB = Script_GetValue();
        p->attackC = Script_GetValue();
    } else {
        p->attackA = 30;
        p->attackB = 10;
        p->attackC = 30;
    }
    if (Script_SeekToKeyword('l')) {
        s32 health = Script_GetValue();
        p->maximumHealth = health;
        p->health = health;
        if ((s16)health < 0)
            p->initialDead = 1;
    } else {
        p->maximumHealth = 120;
        p->health = 120;
    }
    if (Script_SeekToKeyword('d'))
        p->terrain = Script_GetValue();
    if (Script_SeekToKeyword('i'))
        p->duration = Script_GetValue();
    else
        p->duration = 120;
    if (Script_SeekToKeyword('w')) {
        p->delayA = Script_GetValue();
        p->delayB = Script_GetValue();
    } else {
        p->delayA = 180;
        p->delayB = 120;
    }
    if (Script_SeekToKeyword('S'))
        p->overlay = Script_GetValue();
    if (Script_SeekToKeyword('z')) {
        p->damageType = Script_GetValue();
        p->damageFlags = Script_GetValue();
    }
    if (Script_SeekToKeyword('g'))
        p->hudMode = Script_GetValue();
    else
        p->hudMode = 2;
    if (Script_SeekToKeyword('D'))
        p->optionD = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        p->optionG0 = Script_GetValue();
        p->optionG1 = Script_GetValue();
        p->optionG2 = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        p->optionA = Script_GetValue();
        p->optionB = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        p->hudValue = Script_GetValue();
        if (p->hudValue > 99)
            p->hudValue = 99;
        if (p->hudValue <= 0)
            p->hudValue = 1;
    } else
        p->hudValue = 0;
    p->parent = parent;
    AnimData_Load(p->animation, 0x7e7d);
    earlySprite = p->sprite;
    Sprite_Init(earlySprite, p->animation, 1);
    p->position = p->origin;
    AnimData_SelectPaletteBank(p->animation, 0x1b7);
    resource = ScriptProc_Start(0x922e, 0x5184);
    slot = &p->sequence;
    *slot = resource;
    dead = p->initialDead;
    sprite = earlySprite;
    if (dead == 0) {
        p->action = (void (*)(void))ScriptCombatActor_TryProjectileAttack;
        Anim_SetSequence(&p->playback, resource, 0, 0, dead);
    } else {
        p->action = ScriptCombatActor_UpdateDefeatEffect;
        p->dead = 1;
        Anim_SetSequence(&p->playback, p->sequence, 2, 0, 0);
    }
    {
        struct ContactSwarmSprite *frameSprite = sprite;
        ScriptActor_StepAnimation(p, frameSprite, (struct DisplaySequenceState *)&p->playback);
    }
    {
        s16 *initialDead = &p->initialDead;
        if (*initialDead == 0)
            sub_08201784(p);
        if (p->terrain && *initialDead == 0) {
            s32 cell = cellIndexAt((struct Pos16 *)&p->position);
            TerrainModifiers_RegisterCell(p->cell, cell, 0, p->position.y >> 8, 0, 0x803);
        }
    }
    if (p->overlay) {
        u8 *body = p->body;
        SceneBody_Init(body, p->parent, &p->position, 0, 4, p);
        SceneBody_SetSprite(body, sprite);
        TerrainOverlay_QueueAddActor(body);
    }
    p->active = 1;
    p->enabled = 0;
}
