#include "global.h"
#include "display_animation.h"
#include "scene_vec.h"
#include "render.h"
#include "engine.h"
#include "ram.h"
#include "player_cue_sequence.h"
extern u32 gUnk_02000560;
s32 Callback_InvokeActor(void *, void *);
void Collider_SetPosition(void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SceneHud_SetTimedState(void *, s32, s32, s32, s32, s32);
void TerrainModifiers_UnregisterCell(void *);
void sub_08014634(s32, s32, void *, void *, void *, s32, s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void AnimData_SelectPaletteBank(void *, s32);
s32 EnemyD459_UpdateDeath(u8 *);
#define H(o) (*(s16 *)(p + (o)))
#define U(o) (*(u16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
struct ColliderView {
    u8 pad[6];
    u16 flags;
};
struct EffectPair {
    s32 lo : 16;
    u32 hi : 16;
};
struct EffectVector {
    struct EffectPair a, b;
};
static inline u32 TestPair(u32 mask, u32 *a, u32 *b)
{
    return (*a | *b) & mask;
}
static inline u32 TestFlag(u32 mask, u32 *p)
{
    return *p & mask;
}
static inline u32 RandomFrom(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return table[gRandomIndex];
}
s32 EnemyD459_Update(u8 *p)
{
    struct ColliderView *collider;
    if (TestPair(0x400, &gEngineFlags, &gScenePauseFlags) || TestFlag(4, &gUnk_02000560))
        return 0;
    Callback_InvokeActor(p, *(void **)(p + 0x108));
    W(0x24) &= ~1;
    if (H(0x12e) == 0) {
        struct ColliderView *c = (void *)(p + 0x6c);
        c->flags &= ~4;
        Collider_SetPosition(c, p + 0x40, 0);
        collider = c;
    } else {
        struct ColliderView *c = (void *)(p + 0x6c);
        c->flags |= 4;
        collider = c;
    }
    if (H(0x128) > 0 && H(0x12e) == 0) {
        H(0x126) -= H(0x128);
        Sound_PlaySong(0x13e);
        if (H(0x128) > 0) {
            W(0x17c) = W(0x180);
            DamageNumberDisplay_Show(collider, H(0x128), 0, p + 0x78, p + 0x80, 1);
            SceneHud_SetTimedState(*(void **)(p + 0x15c), H(0x126), H(0x138), H(0x13e),
                                   p[0xaf] == 4, H(0x142));
        }
        if (H(0x126) <= 0) {
            struct EffectVector spread, velocity;
            struct SceneVec center;
            const u16 *table;
            s16 *position;
            s16 *z;
            H(0x12e) = 1;
            if (W(0x154))
                TerrainModifiers_UnregisterCell(p + 0x160);
            {
                struct SceneVec *v = &center;
                position = &H(0x40);
                *v = *(struct SceneVec *)position;
                v->y += 200;
            }
            velocity.a.lo = -4;
            table = (const u16 *)RANDOM_TABLE_ADDR;
            velocity.a.hi = RandomFrom(table) & 7;
            velocity.b.lo = -4;
            spread.a.lo = 24;
            spread.a.hi = (RandomFrom(table) & 7) + 3;
            spread.b.lo = 24;
            sub_08014634((RandomFrom(table) & 3) + 4, 13, &center, &velocity, &spread, 15, 15);
            Anim_RestartSequence(p + 0x10c, *(void **)(p + 0x158), 4, 0, 0);
            *position = U(0x1c) + 0x40;
            {
                u16 v = U(0x20) + 0x40;
                z = &H(0x44);
                *z = v;
            }
            AnimData_SelectPaletteBank(p + 0x50, 0x1b7);
            *(u8 **)(p + 0x5c) = gPaletteBankData + 0x44a0;
            H(0x130) = 0;
            *(void **)(p + 0x108) = EnemyD459_UpdateDeath;
            W(0x18) = 0;
            *position -= 0x60;
            *z -= 0x60;
            Sound_PlaySong(0x52b);
        }
        {
            s16 *flash = &H(0x130);
            s32 zero = 0;
            *flash = 8;
            H(0x12a) = 1;
            H(0x128) = zero;
        }
    }
    if (H(0x130) > 0 && H(0x12e) == 0) {
        AnimData_SelectPaletteBank(p + 0x50, 0x192);
        H(0x130)--;
    } else
        AnimData_SelectPaletteBank(p + 0x50, 0x227);
    if (U(0xb0)) {
        U(0xb0)--;
        if ((U(0xb0) >> 2) & 1)
            W(0x24) |= 1;
        else
            W(0x24) &= ~1;
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)(p + 0x24),
                           (struct DisplaySequenceState *)(p + 0x10c));
    W(0x18)++;
}
