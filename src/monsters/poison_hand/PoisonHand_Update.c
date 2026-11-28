#include "effects/particles/enemy_particle.h"
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
void EnemyAttackParticle_InitParameters(void *);
void EnemyAttackParticle_SetGraphicsParameters(void *, u32, u32, s32);
void EnemyAttackParticle_SetMotionParameters(void *, u16, u16, u8, u32);
void EnemyAttackParticle_SetVerticalMotion(void *, s32, s32);
void EnemyAttackParticle_EnableShadow(void *);
void EnemyAttackParticle_AddFlags(void *, u32);
void EnemyAttackParticle_Emit(void *, void *);

s32 PoisonHand_Update(struct PoisonHand *hand)
{
    u16 amount;
    /* The random bank and the final age update share one pointer. */
    void *dataOwner;
    if (Engine_PauseBits(0x400) || Flags_MaskedWord(4, &gPlayerSlotStatus[0]))
        return 0;
    Callback_InvokeActor(hand, hand->update);
    hand->sprite.flags &= ~1;
    if (hand->defeated == 0) {
        struct ContactCollider *collider = &hand->collider;

        collider->flags &= ~4;
        Collider_SetPosition(collider, &hand->sprite.position, 0);
    } else {
        struct ContactCollider *collider = &hand->collider;

        collider->flags |= 4;
    }
    amount = hand->damage;
    if (hand->damage > 0 && hand->defeated == 0) {
        hand->health -= amount;
        hand->reaction = hand->pendingReaction;
        Sound_PlaySong(0x13e);
        if (hand->damage > 0) {
            DamageNumberDisplay_Show(&hand->collider, hand->damage, 0, &hand->collider.position,
                                     &hand->collider.offset, 1);
            SceneHud_SetTimedState(hand->hud, hand->health, hand->maxHealth, hand->hudState,
                                   hand->collider.costFlags == 4, hand->hudTimer);
        }
        if (hand->health <= 0) {
            struct EnemyParticleParams parameters;
            const u16 *randomTable;
            s32 i;
            hand->defeated = 1;
            hand->collider.responseValue = 0;
            for (i = 0; i < 8; i++) {
                dataOwner = gUnk_0203B000;
                randomTable = (const u16 *)dataOwner + 512;
                {
                    s32 frame = Mod(ParticleRandom_ReadSample(randomTable), 2) + 0x1d;
                    u8 direction;
                    EnemyAttackParticle_InitParameters(&parameters);
                    direction = ParticleRandom_ReadSample(randomTable);
                    EnemyAttackParticle_SetGraphicsParameters(&parameters, 0x7e7c, frame, 0x1c5);
                    EnemyAttackParticle_SetMotionParameters(
                        &parameters, 0, 60, direction, ParticleRandom_ReadSample(randomTable) & 15);
                    EnemyAttackParticle_SetVerticalMotion(&parameters, 0x2000, 0x18000);
                    EnemyAttackParticle_EnableShadow(&parameters);
                    if (i & 1)
                        EnemyAttackParticle_AddFlags(&parameters, 0x200);
                    else
                        EnemyAttackParticle_AddFlags(&parameters, 0x400);
                    if (ParticleRandom_ReadBankSample((const u16 *)dataOwner) & 1)
                        EnemyAttackParticle_AddFlags(&parameters, 4);
                    if (ParticleRandom_ReadBankSample((const u16 *)dataOwner) & 1)
                        EnemyAttackParticle_AddFlags(&parameters, 8);
                    EnemyAttackParticle_Emit(&parameters, &hand->sprite.position);
                }
            }
            Anim_RestartSequence(&hand->sequence, hand->animationResource, 0x19, 0, 0);
            AnimData_SelectPaletteBank(&hand->graphics, 0x1c5);
            hand->flashTimer = 0;
            hand->update = PoisonHand_UpdateDefeatBurst;
            hand->age = 0;
            Sound_PlaySong(0x52f);
        }
        hand->flashTimer = 8;
        hand->hitReaction = 1;
        hand->damage = 0;
    }
    if (hand->flashTimer > 0) {
        AnimData_SelectPaletteBank(&hand->graphics, 0x192);
        hand->flashTimer--;
    } else
        AnimData_SelectPaletteBank(&hand->graphics, 0x1c5);
    if (hand->collider.responseValue) {
        hand->collider.responseValue--;
        if ((hand->collider.responseValue >> 2) & 1)
            hand->sprite.flags |= 1;
        else
            hand->sprite.flags &= ~1;
    }
    PlayerCue_StepSequenceEnded(&hand->sprite, &hand->sequence);
    dataOwner = hand;
    ((struct PoisonHand *)dataOwner)->age++;
}
