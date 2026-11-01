#include "global.h"
#include "player_cue_sequence.h"
#include "scene_vec.h"
#include "engine.h"
#include "ram.h"
struct FanSeq {
    const volatile u16 *commands;
    u8 flags, count, weight, duration;
    u16 index, unused, speed, elapsed;
};
struct EffectVector {
    s32 x : 16;
    u32 y : 16;
    s32 z : 16;
};
struct ColliderView {
    u8 pad[6];
    u16 flags;
};
/* Combat view; unidentified HUD values retain their observed offsets. */
struct DragonPuppyCombatView {
    u8 unknown0[0x18];
    u32 ticks;
    u8 unknown1C[0x8];
    u32 spriteFlags;
    u8 unknown28[0x18];
    struct SceneVec position;
    u8 unknown48[0x8];
    u8 palette[0x1C];
    struct ColliderView collider;
    u8 unknown74[0x4];
    u8 hitOrigin[8];
    u8 hitSpread[8];
    u8 unknown88[0x27];
    u8 kind;
    u16 blinkTimer;
    u8 unknownB2[0x56];
    void *update;
    struct FanSeq animation;
    u8 unknown11C[0xA];
    s16 health;
    s16 damage;
    s16 flashEnabled;
    u8 unknown12C[0x2];
    s16 defeated;
    s16 flashTimer;
    u8 deathVariant;
    u8 unknown133[0x5];
    s16 maxHealth;
    u8 unknown13A[0x6];
    s16 hudValue140;
    u8 unknown142[0x6];
    s16 hudValue148;
    u8 unknown14A[0x2];
    void *context;
    u32 stateWord;
    u32 savedStateWord;
    u8 unknown158[0x18];
    void *deathAnimation;
};

/* Preserve byte-pointer access used by the actor interface. */
#define PUPPY_FIELD(field)                                                                         \
    (*(typeof(((struct DragonPuppyCombatView *)0)                                                  \
                  ->field) *)(p + (u32) & ((struct DragonPuppyCombatView *)0)->field))

extern u32 gUnk_02000560;
void Sound_PlaySong(u32);
void Collider_SetPosition(void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneHud_SetTimedState(void *, s32, s32, s32, u32, s32);
void sub_08014634(s32, s32, void *, void *, void *, s32, s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void DragonPuppy_UpdateAttackWindup(void);
static inline u32 Paused(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}
static inline u32 RandomFrom(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return table[gRandomIndex];
}

s32 DragonPuppy_UpdateCombat(u8 *p)
{
    struct SceneVec position;
    struct EffectVector velocity;
    struct EffectVector size;
    struct ColliderView *collider;
    if (Paused(0x400))
        goto paused;
    {
        u32 mask = 4;
        if (gUnk_02000560 & mask) {
        paused:
            return 0;
        }
    }
    Callback_InvokeActor(p, PUPPY_FIELD(update));
    PUPPY_FIELD(spriteFlags) &= ~1;
    if ((PUPPY_FIELD(defeated)) == 0) {
        struct ColliderView *c = (void *)(((u8 *)&PUPPY_FIELD(collider)));
        c->flags &= 0xfffb;
        Collider_SetPosition(c, ((u8 *)&PUPPY_FIELD(position)), 0);
        collider = c;
    } else {
        struct ColliderView *c = (void *)(((u8 *)&PUPPY_FIELD(collider)));
        c->flags |= 4;
        collider = c;
    }
    if (((PUPPY_FIELD(damage)) > 0) && ((PUPPY_FIELD(defeated)) == 0)) {
        PUPPY_FIELD(health) -= PUPPY_FIELD(damage);
        PUPPY_FIELD(stateWord) = PUPPY_FIELD(savedStateWord);
        Sound_PlaySong(0x13e);
        if ((PUPPY_FIELD(damage)) > 0) {
            DamageNumberDisplay_Show(collider, PUPPY_FIELD(damage), 0,
                                     ((u8 *)&PUPPY_FIELD(hitOrigin)),
                                     ((u8 *)&PUPPY_FIELD(hitSpread)), 1);
            SceneHud_SetTimedState(PUPPY_FIELD(context), PUPPY_FIELD(health),
                                   PUPPY_FIELD(maxHealth), PUPPY_FIELD(hudValue140),
                                   PUPPY_FIELD(kind) == 4, PUPPY_FIELD(hudValue148));
        }
        if ((PUPPY_FIELD(health)) <= 0) {
            const u16 *table;
            s32 count;
            PUPPY_FIELD(defeated) = 1;
            Sound_PlaySong(0x530);
            {
                struct SceneVec *v = &position;
                *v = PUPPY_FIELD(position);
                v->y += 200;
            }
            velocity.x = -4;
            table = (const u16 *)0x0203B400;
            velocity.y = RandomFrom(table) & 3;
            velocity.z = -4;
            size.x = 24;
            size.y = (RandomFrom(table) & 3) + 1;
            size.z = 24;
            count = (RandomFrom(table) & 3) + 4;
            sub_08014634(count, 0x16, &position, &velocity, &size, 15, 15);
            Anim_RestartSequence(((u8 *)&PUPPY_FIELD(animation)), PUPPY_FIELD(deathAnimation), 5,
                                 PUPPY_FIELD(deathVariant), 0);
            AnimData_SelectPaletteBank(((u8 *)&PUPPY_FIELD(palette)), 0x1b9);
            PUPPY_FIELD(flashTimer) = 0;
            PUPPY_FIELD(update) = DragonPuppy_UpdateAttackWindup;
            PUPPY_FIELD(ticks) = 0;
        }
        {
            s16 *flash = &(PUPPY_FIELD(flashTimer));
            s32 zero = 0;
            *flash = 8;
            PUPPY_FIELD(flashEnabled) = 1;
            PUPPY_FIELD(damage) = zero;
        }
    }
    if ((PUPPY_FIELD(flashTimer)) > 0) {
        AnimData_SelectPaletteBank(((u8 *)&PUPPY_FIELD(palette)), 0x192);
        (PUPPY_FIELD(flashTimer))--;
    } else
        AnimData_SelectPaletteBank(((u8 *)&PUPPY_FIELD(palette)), 0x1b9);
    if (PUPPY_FIELD(blinkTimer)) {
        (PUPPY_FIELD(blinkTimer))--;
        if (((PUPPY_FIELD(blinkTimer)) >> 2) & 1)
            PUPPY_FIELD(spriteFlags) |= 1;
        else
            PUPPY_FIELD(spriteFlags) &= ~1;
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&PUPPY_FIELD(spriteFlags),
                           (struct DisplaySequenceState *)&PUPPY_FIELD(animation));
    (PUPPY_FIELD(ticks))++;
}
