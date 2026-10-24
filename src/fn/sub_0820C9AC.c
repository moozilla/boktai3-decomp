// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: pause branch layout, random-table lifetimes, stack slots and explicit callback return.
#include "screamer_playback.h"
extern u32 gUnk_030053F0, gUnk_02000560;
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
    return (gUnk_030053F4 | gUnk_030053F0) & mask;
}
static inline u32 TestFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline u32 RandomFrom(const u16 *table)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
    return table[gUnk_03005308];
}
static inline u32 RandomValue(void)
{
    return RandomFrom((const u16 *)RANDOM_TABLE_ADDR);
}
void Screamer_UpdateDamageParticles(struct ScreamerState *p)
{
    struct ScreamerParticleParameters parameters;
    struct ActorPosition position;
    struct ActorPosition *offsetPosition;
    s32 i;
    void *graphics;
    u16 *blink;
    register u32 allocationToken asm("r0");
    s32 t1;
    t1 = 4;
    if (PauseFlags(0x400) || TestFlags(&gUnk_02000560, t1))
        return;
    Callback_InvokeActor(p, p->update);
    graphics = p->graphics;
    if ((p->f1DC > 0) && (!p->f1E0)) {
        p->f1E4 -= p->f1DC;
        p->f200 = p->f204;
        Sound_PlaySong(0x13E);
        if (p->f1DC > 0) {
            DamageNumberDisplay_Show(p->bodyCollider, p->f1DC, 0, ((u8 *)p) + 0xCC,
                                     ((u8 *)p) + 0xD4, 1);
            SceneHud_SetTimedState(p->f1F8, p->f1E4, p->f1E6, p->f1EC, (*(((u8 *)p) + 0x103)) == 4,
                                   p->f1F4);
        }
        if (p->f1E4 <= 0) {
            register const u16 *randomTable asm("r10");
            p->f1E0 = 1;
            {
                u16 *d1 = (u16 *)(((u8 *)p) + 0xFC);
                d1[4] = 0;
            }
            offsetPosition = &position;
            asm("" : "+r"(&position));
            randomTable = (const u16 *)(gUnk_0203B000 + 0x400);
            asm("" : "+r"(randomTable));
            for (i = 0; i <= 7; i++) {
                s32 frame = Mod(RandomFrom(randomTable), 4) + 0x18;
                u8 direction;
                EnemyParticle_InitParameters(&parameters);
                direction = RandomFrom(randomTable);
                EnemyParticle_SetGraphicsParameters(&parameters, 0x7E7C, frame, 0x1C4);
                EnemyParticle_SetMotionParameters(&parameters, 0, 60, direction,
                                                  RandomFrom(randomTable) & 15);
                EnemyParticle_SetVerticalMotion(&parameters, 0x2000, 0x18000);
                EnemyParticle_EnableShadow(&parameters);
                if (i & 1)
                    EnemyParticle_AddFlags(&parameters, 0x200);
                else
                    EnemyParticle_AddFlags(&parameters, 0x400);
                if (RandomValue() & 1)
                    EnemyParticle_AddFlags(&parameters, 4);
                if (RandomValue() & 1)
                    EnemyParticle_AddFlags(&parameters, 8);
                position = p->sprite.position;
                if (!p->f1D8)
                    (&position)->x += 16;
                else
                    (&position)->z += 16;
                EnemyParticle_Emit(&parameters, &position);
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
        asm("" : "=r"(allocationToken));
        p->f1DE--;
    } else {
        AnimData_SelectPaletteBank(p->graphics, 0x1C4);
        asm("" : "=r"(allocationToken));
    }
    blink = (u16 *)(((u8 *)p) + 0x104);
    if ((*blink) && (!p->f1E0)) {
        (*blink)--;
        if (((*blink) >> 2) & 1)
            p->sprite.flags |= 1;
        else
            p->sprite.flags &= ~1;
    }
    p->age++;
    asm("" : : "r"(allocationToken));
    return;
}

#else
INCLUDE_ASM("asm/nonmatching", Screamer_UpdateDamageParticles);
#endif
