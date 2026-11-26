#include "monsters/enemy_particle.h"
#include "player/cutscene/player_cue_sequence.h"
#include "monsters/poison_hand.h"
#include "engine/video/display_sequence_inline.h"
#include "engine/math/scene_vec.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/math/monster_particle_random.h"
#include "engine/engine_pause.h"
#include "engine/word_flags.h"

#include "player/player.h"
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

s32 PoisonHand_Update(struct PoisonHand *p)
{
    struct ContactCollider *collider;
    /* The loop bank and final age update share the current data owner. */
    void *dataOwner;
    if (Engine_PauseBits(0x400) || Flags_MaskedWord(4, &gPlayerSlotStatus[0]))
        return 0;
    Callback_InvokeActor(p, p->update);
    p->sprite.flags &= ~1;
    if (p->defeated == 0) {
        struct ContactCollider *c = &p->collider;
        c->flags &= ~4;
        Collider_SetPosition(c, &p->sprite.position, 0);
        collider = c;
    } else {
        struct ContactCollider *c = &p->collider;
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
                DamageNumberDisplay_Show(collider, *damage, 0, &p->collider.position,
                                         &p->collider.offset, 1);
                SceneHud_SetTimedState(p->hud, *health, p->maxHealth, p->hudState,
                                       p->collider.costFlags == 4, p->hudTimer);
            }
            if (p->health <= 0) {
                struct EnemyParticleParams parameters;
                const u16 *randomTable;
                s32 i;
                p->defeated = 1;
                p->collider.responseValue = 0;
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
                        EnemyParticle_Emit(&parameters, &p->sprite.position);
                    }
                }
                Anim_RestartSequence(&p->sequence, p->animationResource, 0x19, 0, 0);
                AnimData_SelectPaletteBank(p->graphics, 0x1c5);
                p->flashTimer = 0;
                p->update = PoisonHand_UpdateDefeatBurst;
                p->age = 0;
                Sound_PlaySong(0x52f);
            }
            p->flashTimer = 8;
            p->hitReaction = 1;
            p->damage = 0;
        }
    }
    if (p->flashTimer > 0) {
        AnimData_SelectPaletteBank(p->graphics, 0x192);
        p->flashTimer--;
    } else
        AnimData_SelectPaletteBank(p->graphics, 0x1c5);
    if (p->collider.responseValue) {
        p->collider.responseValue--;
        if ((p->collider.responseValue >> 2) & 1)
            p->sprite.flags |= 1;
        else
            p->sprite.flags &= ~1;
    }
    PlayerCue_StepSequenceEnded((void *)&p->sprite, &p->sequence);
    dataOwner = p;
    ((struct PoisonHand *)dataOwner)->age++;
}
