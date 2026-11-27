#include "scene/collision/contact_collider.h"
#include "effects/particles/enemy_particle.h"
#include "global.h"
#include "monsters/screamer/screamer_state.h"
#include "engine/ram.h"
#include "engine/math/monster_particle_random.h"
#include "engine/engine_pause.h"
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
static inline u32 TestFlags(s16 mask, u32 *flags)
{
    return *flags & mask;
}
static inline void Screamer_CopyPosition(struct ActorPosition *out, const struct ActorPosition *in)
{
    *out = *in;
}
s32 Screamer_UpdateDamageParticles(struct ScreamerState *p)
{
    struct EnemyParticleParams parameters;
    struct ActorPosition position;
    s32 emitted;
    void *dataOwner;
    u16 *blink;
    if (Engine_PauseBits(0x400) || TestFlags(4, &gPlayerSlotStatus[0]))
        return 0;
    Callback_InvokeActor(p, p->update);
    if ((p->f1DC > 0) && (!p->f1E0)) {
        p->f1E4 -= p->f1DC;
        p->f200 = p->f204;
        Sound_PlaySong(0x13e);
        if (p->f1DC > 0) {
            struct ContactCollider *body = (void *)p->bodyCollider;
            DamageNumberDisplay_Show(body, p->f1DC, 0, &body->position, &body->offset, 1);
            SceneHud_SetTimedState(p->f1F8, p->f1E4, p->f1E6, p->f1EC, body->costFlags == 4,
                                   p->f1F4);
        }
        if (p->f1E4 <= 0) {
            const u16 *randomTable;
            p->f1E0 = 1;
            p->blinkTimer = 0;

            for (emitted = 0; emitted < 8; emitted++) {
                dataOwner = gUnk_0203B000;
                randomTable = (const u16 *)dataOwner + 512;
                {
                    s32 frame = Mod(ParticleRandom_ReadSample(randomTable), 4) + 0x18;
                    u8 direction;
                    EnemyAttackParticle_InitParameters(&parameters);
                    direction = ParticleRandom_ReadDirectSample(randomTable);
                    EnemyAttackParticle_SetGraphicsParameters(&parameters, 0x7E7C, frame, 0x1C4);
                    EnemyAttackParticle_SetMotionParameters(&parameters, 0, 60, direction,
                                                      ParticleRandom_ReadSample(randomTable) & 15);
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
                    Screamer_CopyPosition(&position, &p->sprite.position);
                    if (!p->f1D8)
                        position.x += 16;
                    else
                        position.z += 16;
                    EnemyAttackParticle_Emit(&parameters, &position);
                }
            }
            p->f1DE = 0;
            p->update = SceneDrawablePair_SetFlags;
            p->age = 0;
            Screamer_HandleDefeat(p);
            Sound_PlaySong(0x52E);
        } else
            p->f1DE = 8;
        p->f1E2 = 1;
        p->f1DC = 0;
    }
    if (p->f1DE > 0) {
        AnimData_SelectPaletteBank(p->graphics, 0x192);

        p->f1DE--;
    } else {
        AnimData_SelectPaletteBank(p->graphics, 0x1C4);
    }
    blink = &p->blinkTimer;
    if ((*blink) && (!p->f1E0)) {
        (*blink)--;
        if (((*blink) >> 2) & 1)
            p->sprite.flags |= 1;
        else
            p->sprite.flags &= ~1;
    }
    dataOwner = p;
    ((struct ScreamerState *)dataOwner)->age++;
}
