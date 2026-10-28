// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "display_animation.h"
#include "scene_vec.h"
#include "render.h"
#include "engine.h"
#include "ram.h"
// confidence=low: handles collision damage, defeat effects, palette flashing and animation.
#define OrangeSpine_UpdateDamageState sub_0820F378
#ifdef NONMATCHING
// fuzzy: 83.6%
// Remaining: burst-vector packing, pointer lifetimes and animation register allocation.
extern u32 gUnk_02000560;
s32 Callback_InvokeActor(void *, void *);
void Collider_SetPosition(void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SceneHud_SetTimedState(void *, s32, s32, s32, s32, s32);
void TerrainModifiers_UnregisterCell(void *);
void sub_08014634(s32, s32, void *, void *, void *, s32, s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void AnimData_SelectPaletteBank(void *, s32);
void OrangeSpine_UpdateDefeatEffect(u8 *);
void Explosion_CreateAt(void *, s32, s32, s32, s32, s32, s32, s32);
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
static inline void StepAnimation(u8 *p, void *sprite, struct DisplaySequenceState *s)
{
    u32 flags, elapsed;
    u32 duration;
    register u32 mask asm("r6");
    register u32 savedDuration asm("r4");
    const volatile u16 *c = s->commands + s->index;
    {
        u32 frame;
        frame = *c >> 6;
        *(u16 *)((u8 *)sprite + 0x10) = frame;
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
        if (first != (((*c & 0x30) >> 4) & bit))
            *(u32 *)sprite |= 8;
        else
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
                        s->index = s->frameCount - 1;
                }
            } else
                s->index = i - 1;
        } else {
            u32 i = s->index + 1, n;
            s->index = i;
            n = s->frameCount;
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
        s->frameWeight = *c & 15;
        {
            register s32 w asm("r2") = s->frameWeight;
            s32 speed = s->speed;
            speed = (speed * w) >> 6;
            s->duration = speed;
            if (!(u8)speed)
                s->duration = 1;
        }
    }
}
s32 OrangeSpine_UpdateDamageState(u8 *p)
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
        W(0x150) = W(0x154);
        Sound_PlaySong(0x13e);
        if (H(0x128) > 0) {
            DamageNumberDisplay_Show(collider, H(0x128), 0, p + 0x78, p + 0x80, 1);
            SceneHud_SetTimedState(*(void **)(p + 0x14c), H(0x126), H(0x138), H(0x140),
                                   p[0xaf] == 4, H(0x148));
        }
        if (H(0x126) <= 0) {
            struct SceneVec center, smoke;
            struct EffectVector velocity, spread;
            const u16 *table;
            H(0x12e) = 1;
            H(0xb0) = 0;
            center = *(struct SceneVec *)(p + 0x40);
            center.y += 128;
            velocity.a.lo = -4;
            table = gRandomTable;
            velocity.a.hi = RandomFrom(table) & 7;
            velocity.b.lo = -4;
            spread.a.lo = 24;
            spread.a.hi = (RandomFrom(table) & 7) + 3;
            spread.b.lo = 24;
            sub_08014634((RandomFrom(table) & 3) + 4, 13, &center, &velocity, &spread, 15, 15);
            smoke = *(struct SceneVec *)(p + 0x40);
            smoke.y += 128;
            Explosion_CreateAt(&smoke, 16, 0, 0, 0, 60, 0, 0);
            smoke.x += 128;
            Explosion_CreateAt(&smoke, 16, 0, 0, 0, 60, 0, 2);
            smoke.x -= 128;
            smoke.z += 128;
            Explosion_CreateAt(&smoke, 16, 0, 0, 0, 60, 0, 2);
            smoke.z -= 128;
            smoke.x -= 128;
            Explosion_CreateAt(&smoke, 16, 0, 0, 0, 60, 0, 5);
            smoke.x += 128;
            smoke.z -= 128;
            Explosion_CreateAt(&smoke, 16, 0, 0, 0, 60, 0, 5);
            Anim_RestartSequence(p + 0x10c, *(void **)(p + 0x170), 16, p[0x132], 0);
            AnimData_SelectPaletteBank(p + 0x50, 0x1c1);
            H(0x130) = 0;
            *(void **)(p + 0x108) = OrangeSpine_UpdateDefeatEffect;
            W(0x18) = 0;
            Sound_PlaySong(0x52d);
        }
        {
            s16 *flash = &H(0x130);
            s32 zero = 0;
            *flash = 8;
            H(0x12a) = 1;
            H(0x128) = zero;
        }
    }
    if (H(0x130) > 0) {
        AnimData_SelectPaletteBank(p + 0x50, 0x192);
        H(0x130)--;
    } else
        AnimData_SelectPaletteBank(p + 0x50, 0x1c1);
    if (U(0xb0)) {
        U(0xb0)--;
        if ((U(0xb0) >> 2) & 1)
            W(0x24) |= 1;
        else
            W(0x24) &= ~1;
    }
    StepAnimation(p, p + 0x24, (void *)(p + 0x10c));
    W(0x18)++;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0820F378);
#endif
