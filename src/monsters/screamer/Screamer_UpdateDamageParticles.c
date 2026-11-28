#include "scene/collision/contact_collider.h"
#include "effects/particles/enemy_particle.h"
#include "global.h"
#include "monsters/screamer/screamer_state.h"
#include "engine/ram.h"
#include "engine/math/monster_particle_random.h"
#include "engine/engine_pause.h"
#include "engine/word_flags.h"
#include "player/player.h"
extern u8 gUnk_0203B000[];
void SceneDrawablePair_SetFlags(void);
void Screamer_HandleDefeat(void *);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneHud_SetTimedState(void *, s32, s32, s32, u32, s32);
s32 Mod(s32, s32);
void EnemyAttackParticle_InitParameters(void *);
void EnemyAttackParticle_SetGraphicsParameters(void *, u32, u32, s32);
void EnemyAttackParticle_SetMotionParameters(void *, u16, u16, u8, u32);
void EnemyAttackParticle_SetVerticalMotion(void *, s32, s32);
void EnemyAttackParticle_EnableShadow(void *);
void EnemyAttackParticle_AddFlags(void *, u32);
void EnemyAttackParticle_Emit(void *, void *);
static inline void Screamer_CopyPosition(struct ContactPosition *out,
                                         const struct ContactPosition *in)
{
    *out = *in;
}
s32 Screamer_UpdateDamageParticles(struct ScreamerState *screamer)
{
    struct EnemyParticleParams parameters;
    struct ContactPosition position;
    s32 emitted;
    void *dataOwner;
    u16 amount;
    if (Engine_PauseBits(0x400) || Flags_MaskedWord(4, &gPlayerSlotStatus[0]))
        return 0;
    Callback_InvokeActor(screamer, screamer->update);
    amount = screamer->f1DC;
    if (screamer->f1DC > 0 && !screamer->f1E0) {
        screamer->f1E4 -= amount;
        screamer->f200 = screamer->f204;
        Sound_PlaySong(0x13e);
        if (screamer->f1DC > 0) {
            DamageNumberDisplay_Show(&screamer->bodyCollider, screamer->f1DC, 0,
                                     &screamer->bodyCollider.position,
                                     &screamer->bodyCollider.offset, 1);
            SceneHud_SetTimedState(screamer->f1F8, screamer->f1E4, screamer->f1E6, screamer->f1EC,
                                   screamer->bodyCollider.costFlags == 4, screamer->f1F4);
        }
        if (screamer->f1E4 <= 0) {
            const u16 *randomTable;
            screamer->f1E0 = 1;
            screamer->bodyCollider.responseValue = 0;

            for (emitted = 0; emitted < 8; emitted++) {
                dataOwner = gUnk_0203B000;
                randomTable = (const u16 *)dataOwner + 512;
                {
                    s32 frame = Mod(ParticleRandom_ReadSample(randomTable), 4) + 0x18;
                    u8 direction;
                    EnemyAttackParticle_InitParameters(&parameters);
                    direction = ParticleRandom_ReadDirectSample(randomTable);
                    EnemyAttackParticle_SetGraphicsParameters(&parameters, 0x7E7C, frame, 0x1C4);
                    EnemyAttackParticle_SetMotionParameters(
                        &parameters, 0, 60, direction, ParticleRandom_ReadSample(randomTable) & 15);
                    EnemyAttackParticle_SetVerticalMotion(&parameters, 0x2000, 0x18000);
                    EnemyAttackParticle_EnableShadow(&parameters);
                    if (emitted & 1)
                        EnemyAttackParticle_AddFlags(&parameters, 0x200);
                    else
                        EnemyAttackParticle_AddFlags(&parameters, 0x400);
                    if (ParticleRandom_ReadBankSample((const u16 *)dataOwner) & 1)
                        EnemyAttackParticle_AddFlags(&parameters, 4);
                    if (ParticleRandom_ReadBankSample((const u16 *)dataOwner) & 1)
                        EnemyAttackParticle_AddFlags(&parameters, 8);
                    Screamer_CopyPosition(&position, &screamer->sprite.position);
                    if (!screamer->f1D8)
                        position.x += 16;
                    else
                        position.z += 16;
                    EnemyAttackParticle_Emit(&parameters, &position);
                }
            }
            screamer->f1DE = 0;
            screamer->update = SceneDrawablePair_SetFlags;
            screamer->age = 0;
            Screamer_HandleDefeat(screamer);
            Sound_PlaySong(0x52E);
        } else
            screamer->f1DE = 8;
        screamer->f1E2 = 1;
        screamer->f1DC = 0;
    }
    if (screamer->f1DE > 0) {
        AnimData_SelectPaletteBank(&screamer->graphics, 0x192);

        screamer->f1DE--;
    } else {
        AnimData_SelectPaletteBank(&screamer->graphics, 0x1C4);
    }
    if (screamer->bodyCollider.responseValue && !screamer->f1E0) {
        screamer->bodyCollider.responseValue--;
        if ((screamer->bodyCollider.responseValue >> 2) & 1)
            screamer->sprite.flags |= 1;
        else
            screamer->sprite.flags &= ~1;
    }
    dataOwner = screamer;
    ((struct ScreamerState *)dataOwner)->age++;
}
