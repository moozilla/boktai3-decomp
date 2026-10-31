#include "global.h"
#include "display_sequence_inline.h"
#include "scene_vec.h"
#include "engine.h"
#include "ram.h"
#include "monster_particle_random.h"

struct PoisonHandSprite {
    u32 flags;
    u8 unk_04[0xc];
    u16 frame;
    u8 unk_12[0xa];
};
struct PoisonHandCollider {
    u8 unk_00[6];
    u16 flags;
    u8 unk_08[4];
    u8 damagePosition[8];
    u8 damageOffset[8];
    u8 unk_1C[0x27];
    u8 direction;
};
/* This actor has private state at 0x108, inside the canonical actor prefix. */
struct PoisonHand {
    u8 unk_00[0x18];
    u32 age;
    u8 unk_1C[8];
    struct PoisonHandSprite sprite;
    struct SceneVec position;
    u8 unk_48[8];
    u8 graphics[0x1c];
    struct PoisonHandCollider collider;
    u16 blinkTimer;
    u8 unk_B2[0x56];
    void *update;
    struct DisplaySequenceState sequence;
    u8 unk_11C[0xe];
    s16 flashTimer;
    u8 unk_12C[4];
    s16 hitReaction;
    s16 defeated;
    s16 damage;
    s16 health;
    s16 maxHealth;
    u8 unk_13A[4];
    s16 hudState;
    u8 unk_140[2];
    s16 hudTimer;
    u8 unk_144[8];
    void *hud;
    u32 reaction;
    u32 pendingReaction;
    u8 unk_158[0x10];
    void *animationResource;
};
struct ParticleParameters {
    u32 words[14];
};
extern u32 gUnk_02000560;
extern u8 gUnk_0203B000[];
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
void Collider_SetPosition(void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneHud_SetTimedState(void *, s32, s32, s32, u32, s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void AnimData_SelectPaletteBank(void *, s32);
s32 PoisonHand_UpdateDefeatBurst(void *);
s32 Mod(s32, s32);
void EnemyParticle_InitParameters(void *);
void EnemyParticle_SetGraphicsParameters(void *, u32, u32, s32);
void EnemyParticle_SetMotionParameters(void *, u16, u16, u8, u32);
void EnemyParticle_SetVerticalMotion(void *, s32, s32);
void EnemyParticle_EnableShadow(void *);
void EnemyParticle_AddFlags(void *, u32);
void EnemyParticle_Emit(void *, void *);

static inline u32 TestPair(u32 mask, u32 *a, u32 *b)
{
    return (*a | *b) & mask;
}
static inline u32 TestFlag(u32 mask, u32 *p)
{
    return *p & mask;
}
static inline u32 PoisonHand_StepSequence(struct PoisonHandSprite *owner,
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
            owner->flags |= 4;
        else
            owner->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 8;
        else
            owner->flags &= ~8;
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

s32 PoisonHand_Update(struct PoisonHand *p)
{
    struct PoisonHandCollider *collider;
    /* The loop bank and final age update share the current data owner. */
    void *dataOwner;
    if (TestPair(0x400, &gEngineFlags, &gScenePauseFlags) || TestFlag(4, &gUnk_02000560))
        return 0;
    Callback_InvokeActor(p, p->update);
    p->sprite.flags &= ~1;
    if (p->defeated == 0) {
        struct PoisonHandCollider *c = &p->collider;
        c->flags &= ~4;
        Collider_SetPosition(c, &p->position, 0);
        collider = c;
    } else {
        struct PoisonHandCollider *c = &p->collider;
        c->flags |= 4;
        collider = c;
    }
    {
        s16 *damage = &p->damage;
        u16 amount = *damage;
        if (*damage > 0 && p->defeated == 0) {
            s16 *health = &p->health;
            *health -= amount;
            p->reaction = p->pendingReaction;
            Sound_PlaySong(0x13e);
            if (*damage > 0) {
                DamageNumberDisplay_Show(collider, *damage, 0, p->collider.damagePosition,
                                         p->collider.damageOffset, 1);
                SceneHud_SetTimedState(p->hud, *health, p->maxHealth, p->hudState,
                                       p->collider.direction == 4, p->hudTimer);
            }
            if (p->health <= 0) {
                struct ParticleParameters parameters;
                const u16 *randomTable;
                s32 i;
                p->defeated = 1;
                p->blinkTimer = 0;
                i = 0;
                for (; i <= 7; i++) {
                    dataOwner = gUnk_0203B000;
                    randomTable = (const u16 *)dataOwner + 512;
                    {
                        s32 frame = Mod(ParticleRandom_ReadSample(randomTable), 2) + 0x1d;
                        u8 direction;
                        EnemyParticle_InitParameters(&parameters);
                        direction = ParticleRandom_ReadSample(randomTable);
                        EnemyParticle_SetGraphicsParameters(&parameters, 0x7e7c, frame, 0x1c5);
                        EnemyParticle_SetMotionParameters(&parameters, 0, 60, direction,
                                                          ParticleRandom_ReadSample(randomTable) &
                                                              15);
                        EnemyParticle_SetVerticalMotion(&parameters, 0x2000, 0x18000);
                        EnemyParticle_EnableShadow(&parameters);
                        if (i & 1)
                            EnemyParticle_AddFlags(&parameters, 0x200);
                        else
                            EnemyParticle_AddFlags(&parameters, 0x400);
                        {
                            const u16 *lateBank = (const u16 *)dataOwner;
                            if (ParticleRandom_ReadBankSample(lateBank) & 1)
                                EnemyParticle_AddFlags(&parameters, 4);
                            if (ParticleRandom_ReadBankSample(lateBank) & 1)
                                EnemyParticle_AddFlags(&parameters, 8);
                        }
                        EnemyParticle_Emit(&parameters, &p->position);
                    }
                }
                Anim_RestartSequence(&p->sequence, p->animationResource, 0x19, 0, 0);
                AnimData_SelectPaletteBank(p->graphics, 0x1c5);
                p->flashTimer = 0;
                p->update = PoisonHand_UpdateDefeatBurst;
                p->age = 0;
                Sound_PlaySong(0x52f);
            }
            {
                s16 *flash = &p->flashTimer;
                s32 zero = 0;
                *flash = 8;
                p->hitReaction = 1;
                p->damage = zero;
            }
        }
    }
    if (p->flashTimer > 0) {
        AnimData_SelectPaletteBank(p->graphics, 0x192);
        p->flashTimer--;
    } else
        AnimData_SelectPaletteBank(p->graphics, 0x1c5);
    if (p->blinkTimer) {
        p->blinkTimer--;
        if ((p->blinkTimer >> 2) & 1)
            p->sprite.flags |= 1;
        else
            p->sprite.flags &= ~1;
    }
    PoisonHand_StepSequence(&p->sprite, &p->sequence);
    dataOwner = p;
    ((struct PoisonHand *)dataOwner)->age++;
}
