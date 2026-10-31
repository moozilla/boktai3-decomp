#include "global.h"
#include "screamer_state.h"
#include "ram.h"
#include "monster_particle_random.h"
extern u32 gUnk_02000560;
extern u8 gUnk_0203B000[];
void SceneDrawablePair_SetFlags(void);
void Screamer_HandleDefeat(void *);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneHud_SetTimedState(void *, s32, s32, s32, u32, s32);
s32 Mod(s32, s32);
void EnemyParticle_InitParameters(void *);
void EnemyParticle_SetGraphicsParameters(void *, u32, u32, s32);
void EnemyParticle_SetMotionParameters(void *, u16, u16, u8, u32);
void EnemyParticle_SetVerticalMotion(void *, s32, s32);
void EnemyParticle_EnableShadow(void *);
void EnemyParticle_AddFlags(void *, u32);
void EnemyParticle_Emit(void *, void *);
struct ScreamerParticleParameters {
    u32 data[14];
};
static inline u32 PauseFlags(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}
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
    struct ScreamerParticleParameters parameters;
    struct ActorPosition position;
    s32 emitted;
    void *dataOwner;
    u16 *blink;
    if (PauseFlags(0x400) || TestFlags(4, &gUnk_02000560))
        return 0;
    Callback_InvokeActor(p, p->update);
    if ((p->f1DC > 0) && (!p->f1E0)) {
        p->f1E4 -= p->f1DC;
        p->f200 = p->f204;
        Sound_PlaySong(0x13e);
        if (p->f1DC > 0) {
            DamageNumberDisplay_Show(p->bodyCollider, p->f1DC, 0, ((u8 *)p) + 0xCC,
                                     ((u8 *)p) + 0xD4, 1);
            SceneHud_SetTimedState(p->f1F8, p->f1E4, p->f1E6, p->f1EC, (*(((u8 *)p) + 0x103)) == 4,
                                   p->f1F4);
        }
        if (p->f1E4 <= 0) {
            const u16 *randomTable;
            p->f1E0 = 1;
            *(u16 *)((u8 *)p + 0x104) = 0;

            emitted = 0;

            for (; emitted <= 7; emitted++) {
                dataOwner = gUnk_0203B000;
                randomTable = (const u16 *)dataOwner + 512;
                {
                    s32 frame = Mod(ParticleRandom_ReadSample(randomTable), 4) + 0x18;
                    u8 direction;
                    EnemyParticle_InitParameters(&parameters);
                    direction = ParticleRandom_ReadDirectSample(randomTable);
                    EnemyParticle_SetGraphicsParameters(&parameters, 0x7E7C, frame, 0x1C4);
                    EnemyParticle_SetMotionParameters(&parameters, 0, 60, direction,
                                                      ParticleRandom_ReadSample(randomTable) & 15);
                    EnemyParticle_SetVerticalMotion(&parameters, 0x2000, 0x18000);
                    EnemyParticle_EnableShadow(&parameters);
                    if (emitted & 1)
                        EnemyParticle_AddFlags(&parameters, 0x200);
                    else
                        EnemyParticle_AddFlags(&parameters, 0x400);
                    if (ParticleRandom_ReadBankSample((const u16 *)dataOwner) & 1)
                        EnemyParticle_AddFlags(&parameters, 4);
                    if (ParticleRandom_ReadBankSample((const u16 *)dataOwner) & 1)
                        EnemyParticle_AddFlags(&parameters, 8);
                    Screamer_CopyPosition(&position, &p->sprite.position);
                    if (!p->f1D8)
                        (&position)->x += 16;
                    else
                        (&position)->z += 16;
                    EnemyParticle_Emit(&parameters, &position);
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
    blink = (u16 *)(((u8 *)p) + 0x104);
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
