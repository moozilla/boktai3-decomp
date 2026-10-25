// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// twin.py transplant from a matched near-clone
// fix.py best version (reprinted C)
// Remaining differences: random-table constant registers (0xFFFF0000/0xFFFC/3/0x3FF),
// the collider pointer copy in the disabled-collider branch, and the resulting branch reach.
#include "scene_vec.h"
struct FanSeq {
    const volatile u16 *commands;
    u8 flags, count, weight, duration;
    u16 index, unused, speed, elapsed;
};
struct EffectVector {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
struct ColliderView {
    u8 pad[6];
    u16 flags;
};
extern u32 gUnk_030053F0, gUnk_02000560, gUnk_03005308, gUnk_030053F4;
void Sound_PlaySong(u32);
void Collider_SetPosition(void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneHud_SetTimedState(void *, s32, s32, s32, u32, s32);
void sub_08014634(s32, s32, void *, void *, void *, s32, s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void DragonPuppy_UpdateAttackWindup(void);
#define H(o) (*(s16 *)(p + (o)))
#define U(o) (*(u16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
#define B(o) (*(u8 *)(p + (o)))
static inline u32 Paused(u32 mask)
{
    return (gUnk_030053F4 | gUnk_030053F0) & mask;
}
static inline u32 RandomFrom(const u16 *table)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
    return table[gUnk_03005308];
}
static inline void DragonPuppy_StepAnimation(u8 *p, void *sprite, struct FanSeq *s)
{
    u32 flags, elapsed;
    u32 duration;
    register u32 mask asm("r6");
    register u32 savedDuration asm("r4");
    const volatile u16 *c = s->commands + s->index;
    *(u16 *)((u8 *)sprite + 0x10) = *c >> 6;
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
                        s->index = s->count - 1;
                }
            } else
                s->index = i - 1;
        } else {
            u32 i = s->index + 1, n;
            s->index = i;
            n = s->count;
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
        s->weight = *c & 15;
        {
            register s32 w asm("r2") = s->weight;
            s32 speed = s->speed;
            speed = (speed * w) >> 6;
            s->duration = speed;
            if (!(u8)speed)
                s->duration = 1;
        }
    }
}
static inline void DragonPuppy_StepAnimation_UpdateCombat(u8 *p, void *sprite, struct FanSeq *s)
{
    u32 flags;
    u32 elapsed;
    u32 duration;
    register u32 mask asm("r6");
    register u32 savedDuration asm("r4");
    const volatile u16 *c = s->commands + s->index;
    *(u16 *)((u8 *)sprite + 0x10) = *c >> 6;
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
                        s->index = s->count - 1;
                }
            } else
                s->index = i - 1;
        } else {
            u32 i = s->index + 1, n;
            s->index = i;
            n = s->count;
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
        s->weight = *c & 15;
        {
            register s32 w asm("r2") = s->weight;
            s32 speed = s->speed;
            speed = (speed * w) >> 6;
            s->duration = speed;
            if (!(u8)speed)
                s->duration = 1;
        }
    }
}

static inline void DragonPuppy_StepAnimation_UpdateCombat(u8 *p, void *sprite, struct FanSeq *s)
{
    u32 flags;
    u32 elapsed;
    u32 duration;
    register u32 mask asm("r6");
    register u32 savedDuration asm("r4");
    const volatile u16 *c = s->commands + s->index;
    *(u16 *)((u8 *)sprite + 0x10) = *c >> 6;
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
                        s->index = s->count - 1;
                }
            } else
                s->index = i - 1;
        } else {
            u32 i = s->index + 1, n;
            s->index = i;
            n = s->count;
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
        s->weight = *c & 15;
        {
            register s32 w asm("r2") = s->weight;
            s32 speed = s->speed;
            speed = (speed * w) >> 6;
            s->duration = speed;
            if (!(u8)speed)
                s->duration = 1;
        }
    }
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
    Callback_InvokeActor(p, *((void **)(p + 0x108)));
    *((u32 *)(p + 0x24)) &= ~1;
    if ((*((s16 *)(p + 0x12e))) == 0) {
        collider = (void *)(p + 0x6c);
        {
            register u32 flags asm("r1") = collider->flags;
            collider->flags = 0xfffb & flags;
        }
        Collider_SetPosition(collider, p + 0x40, 0);
    } else {
        collider = (void *)(p + 0x6c);
        collider->flags |= 4;
    }
    if (((*((s16 *)(p + 0x128))) > 0) && ((*((s16 *)(p + 0x12e))) == 0)) {
        *((s16 *)(p + 0x126)) -= *((s16 *)(p + 0x128));
        *((u32 *)(p + 0x150)) = *((u32 *)(p + 0x154));
        Sound_PlaySong(0x13e);
        if ((*((s16 *)(p + 0x128))) > 0) {
            DamageNumberDisplay_Show(collider, *((s16 *)(p + 0x128)), 0, p + 0x78, p + 0x80, 1);
            SceneHud_SetTimedState(*((void **)(p + 0x14c)), *((s16 *)(p + 0x126)),
                                   *((s16 *)(p + 0x138)), *((s16 *)(p + 0x140)), p[0xaf] == 4,
                                   *((s16 *)(p + 0x148)));
        }
        if ((*((s16 *)(p + 0x126))) <= 0) {
            const u16 *table;
            s32 count;
            *((s16 *)(p + 0x12e)) = 1;
            Sound_PlaySong(0x530);
            {
                struct SceneVec *v = &position;
                *v = *((struct SceneVec *)(p + 0x40));
                v->y += 200;
            }
            {
                u32 *idxp;
                register u32 r asm("r1");
                velocity.x = -4;
                table = (const u16 *)0x0203B400;
                idxp = &gUnk_03005308;
                {
                    u8 k1 = 0x3ff;
                    r = ((*idxp) + 1) & k1;
                }
                asm("" : "+r"(table), "+r"(idxp));
                velocity.y = table[r] & 3;
                velocity.z = -4;
                size.x = 24;
                r = (r + 1) & 0x3ff;
                size.y = (table[r] & 3) + 1;
                size.z = 24;
                r = (r + 1) & 0x3ff;
                *idxp = r;
                {
                    u32 three = 3;
                    asm("" : "+r"(three));
                    count = (table[r] & three) + 4;
                }
            }
            sub_08014634(count, 0x16, &position, &velocity, &size, 15, 15);
            Anim_RestartSequence(p + 0x10c, *((void **)(p + 0x170)), 5, *((u8 *)(p + 0x132)), 0);
            AnimData_SelectPaletteBank(p + 0x50, 0x1b9);
            *((s16 *)(p + 0x130)) = 0;
            *((void (**)(void))(p + 0x108)) = DragonPuppy_UpdateAttackWindup;
            *((u32 *)(p + 0x18)) = 0;
        }
        {
            s16 *flash = &(*((s16 *)(p + 0x130)));
            s32 zero = 0;
            *flash = 8;
            *((s16 *)(p + 0x12a)) = 1;
            *((s16 *)(p + 0x128)) = zero;
        }
    }
    if ((*((s16 *)(p + 0x130))) > 0) {
        AnimData_SelectPaletteBank(p + 0x50, 0x192);
        (*((s16 *)(p + 0x130)))--;
    } else
        AnimData_SelectPaletteBank(p + 0x50, 0x1b9);
    if (*((u16 *)(p + 0xb0))) {
        (*((u16 *)(p + 0xb0)))--;
        if (((*((u16 *)(p + 0xb0))) >> 2) & 1)
            *((u32 *)(p + 0x24)) |= 1;
        else
            *((u32 *)(p + 0x24)) &= ~1;
    }
    {
        register void *sprite asm("r8") = p + 0x24;
        DragonPuppy_StepAnimation_UpdateCombat(p, sprite, (struct FanSeq *)(p + 0x10c));
    }
    (*((u32 *)(p + 0x18)))++;
}

#else
INCLUDE_ASM("asm/nonmatching", DragonPuppy_UpdateCombat);
#endif
