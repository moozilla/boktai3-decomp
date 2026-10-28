// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_vec.h"
#include "engine_globals.h"
struct ScriptActorVec {
    s16 x, y, z, pad;
};
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
struct ScriptActorTerrain {
    u8 pad[0x24];
    u16 rows[1];
};
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, s32);
void AnimData_SelectPaletteBank(void *, s32);
void *ScriptProc_Start(u32, u32);
void Anim_SetSequence(void *, void *, u32, u32, u32);
void sub_08201784(void *);
u32 ScriptCombatActor_TryProjectileAttack(u8 *);
void ScriptCombatActor_UpdateDefeatEffect(void);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
static inline s32 ScriptActor_CellIndex(struct ScriptActorVec *v)
{
    s32 x = (s16)v->x >> 8, z = (s16)v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return ((struct ScriptActorTerrain *)gTerrainMap)->rows[z] + x;
}
static inline void ScriptActor_StepAnimation(u8 *p, void *sprite, struct ScriptActorSequence *s)
{
    u32 flags, elapsed;
    u32 duration;
    register u32 mask asm("r7");
    register u32 savedDuration asm("r4");
    const volatile u16 *c = s->commands + s->index;
    {
        u32 frame;
        register void *highSprite asm("r8") = sprite;
        register void *frameSprite asm("r3");
        frame = *c >> 6;
        frameSprite = highSprite;
        asm("" : "+r"(frameSprite));
        *(u16 *)((u8 *)frameSprite + 0x10) = frame;
    }
    {
        u32 input = s->flags, bit = 1;
        register u32 first asm("r2");
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*c & 0x30) >> 4) & bit))
            *(u32 *)(p + 0x24) |= 4;
        else
            *(u32 *)(p + 0x24) &= ~4;
    }
    {
        u32 input = s->flags, bit = 2;
        register u32 first asm("r2");
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*c & 0x30) >> 4) & bit)) {
            u32 value = *(u32 *)sprite;
            asm("" : "+r"(value) : : "r3");
            *(u32 *)sprite = value | 8;
        } else
            *(u32 *)sprite &= ~8;
    }
    elapsed = s->elapsed + 1;
    s->elapsed = elapsed;
    {
        register u32 temp asm("r2") = 0xffff;
        asm("" : "+r"(temp));
        mask = temp;
    }
    duration = s->duration;
    elapsed = (u16)elapsed;
    savedDuration = duration;
    asm("" : "+r"(savedDuration) : "r"(elapsed));
    if ((u16)elapsed >= duration) {
        s->elapsed = 0;
        flags = s->flags;
        if (flags & 4) {
            u32 i = s->index;
            if (!i) {
                {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold)
                        s->elapsed = savedDuration;
                    else
                        s->index = s->count - 1;
                }
            } else
                s->index = i - 1;
        } else {
            u32 i = s->index + 1, n;
            s->index = i;
            n = s->count;
            if ((i & mask) >= n) {
                {
                    u32 hold = 8;
                    hold &= flags;
                    if ((u8)hold) {
                        s->elapsed = duration;
                        s->index = n - 1;
                    } else
                        s->index = 0;
                }
            }
        }
        c = s->commands + s->index;
        s->weight = *c & 15;
        {
            register s32 w asm("r2") = s->weight;
            s32 speed = s->speed;
            speed = (speed * w) >> 6;
            s->duration = speed;
            if (!(u8)speed)
                s->duration = 1;
        }
    }
}
// confidence=low: parses actor script options and initializes display, collision and terrain state.
#define ScriptCombatActor_InitializeFromScript sub_0820212C
s32 ScriptCombatActor_InitializeFromScript(struct ScriptActor *p, void *parent)
{
    void *resource;
    register void *sprite asm("r8");
    void *earlySprite;
    register void **slot asm("r4");
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
        {
            u32 zero;
            s16 *q = &p->dead;
            zero = 0;
            *q = 1;
            Anim_SetSequence(&p->playback, ({
                void *r = *slot;
                r;
            }),
                             2, 0, zero);
        }
    }
    ScriptActor_StepAnimation((u8 *)p, sprite, &p->playback);
    {
        register u32 offset asm("r3") = 0x1a8;
        s16 *initialDead;
        asm("" : "+r"(offset));
        initialDead = (s16 *)((u8 *)p + offset);
        if (*initialDead == 0)
            sub_08201784(p);
        if (p->terrain && *initialDead == 0) {
            s32 cell = ScriptActor_CellIndex((struct ScriptActorVec *)(&p->position));
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
