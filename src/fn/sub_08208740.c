// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// twin.py transplant from a matched near-clone
// Collider and sprite views, constant reuse, and the defined zero return still differ.
#include "display_animation.h"
#include "scene_vec.h"
extern u32 gUnk_030053F0, gUnk_02000560, gUnk_03005308;
s32 Mod(s32, s32);
void Sound_PlaySong(u32);
void Collider_SetPosition(void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneHud_SetTimedState(void *, s32, s32, s32, u32, s32);
void sub_08014634(s32, s32, void *, void *, void *, s32, s32);
void sub_082086B8(void);
void CannonTurret_FireAimedVolley(void);
void Anim_SetDurationScale(void *, u16);
#define H(o) (*(s16 *)(p + (o)))
#define U(o) (*(u16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
static inline void CopyPosition(struct SceneVec *out, const struct SceneVec *in)
{
    *out = *in;
}
static inline u32 RandomFrom(const u16 *table)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
    return table[gUnk_03005308];
}
static inline u32 Paused(u32 mask)
{
    return (gUnk_030053F4 | gUnk_030053F0) & mask;
}
struct ColliderView {
    u8 pad[6];
    u16 flags;
};
struct EffectVector {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
static inline u32 StepAnimation(u8 *owner, struct DisplaySequenceState *stateArg, u32 flipMask)
{
    struct DisplaySequenceSprite *sprite = (struct DisplaySequenceSprite *)(owner + 0x24);
    struct DisplaySequenceState *state = stateArg;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    {
        u32 frame = *command >> 6;
        register struct DisplaySequenceSprite *out asm("r3") = sprite;
        out->frame = frame;
    }
    {
        u32 input = state->flags;
        register u32 first asm("r2");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit)) {
            u32 flags = (*(u32 *)(owner + 0x24));
            u32 flip = flipMask;
            (*(u32 *)(owner + 0x24)) = flags | flip;
        } else
            (*(u32 *)(owner + 0x24)) &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r2");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        {
            register u32 flags asm("r1");
            if (first != (((*command & 0x30) >> 4) & bit)) {
                struct DisplaySequenceSprite *read = sprite;
                flags = read->flags | 8;
            } else {
                struct DisplaySequenceSprite *read = sprite;
                flags = read->flags & ~8;
            }
            {
                struct DisplaySequenceSprite *out = sprite;
                asm("" : "+r"(out));
                out->flags = flags;
            }
        }
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r2");
        register u32 savedDuration asm("r4");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */
        asm("" : "+&r"(mask) : "r"(temp));
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                    ended = 1;
                } else {
                    ended = 0;
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r2") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}
static inline u32 StepAnimation_UpdateCombat(u8 *owner, struct DisplaySequenceState *stateArg,
                                             u32 flipMask)
{
    struct DisplaySequenceSprite *sprite = (struct DisplaySequenceSprite *)(owner + 0x24);
    struct DisplaySequenceState *state = stateArg;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended;
    u32 duration;
    u32 flags;
    u32 bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    {
        u32 frame = *command >> 6;
        register struct DisplaySequenceSprite *out asm("r3") = sprite;
        out->frame = frame;
    }
    {
        u32 input = state->flags;
        register u32 first asm("r2");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit)) {
            u32 flags = (*(u32 *)(owner + 0x24));
            u32 flip = flipMask;
            (*(u32 *)(owner + 0x24)) = flags | flip;
        } else
            (*(u32 *)(owner + 0x24)) &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r2");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        {
            register u32 flags asm("r1");
            if (first != (((*command & 0x30) >> 4) & bit)) {
                struct DisplaySequenceSprite *read = sprite;
                flags = read->flags | 8;
            } else {
                struct DisplaySequenceSprite *read = sprite;
                flags = read->flags & ~8;
            }
            {
                struct DisplaySequenceSprite *out = sprite;
                asm("" : "+r"(out));
                out->flags = flags;
            }
        }
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r2");
        register u32 savedDuration asm("r4");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */
        asm("" : "+&r"(mask) : "r"(temp));
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                    ended = 1;
                } else {
                    ended = 0;
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r2") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}

s32 CannonTurret_UpdateCombat(u8 *p)
{
    struct SceneVec position;
    struct EffectVector velocity;
    struct EffectVector size;
    struct ColliderView *collider;
    u32 ended;
    register u32 token asm("r0");
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
    if ((*((s16 *)(p + 0x130))) == 0) {
        struct ColliderView *c = (void *)(p + 0x6c);
        collider->flags &= ~4;
        Collider_SetPosition(collider, p + 0x40, 0);
    } else {
        collider = (void *)(p + 0x6c);
        collider->flags |= 4;
    }
    if (((*((s16 *)(p + 0x12c))) > 0) && ((*((s16 *)(p + 0x130))) == 0)) {
        *((s16 *)(p + 0x132)) -= *((s16 *)(p + 0x12c));
        *((u32 *)(p + 0x14c)) = *((u32 *)(p + 0x150));
        Sound_PlaySong(0x13e);
        if ((*((s16 *)(p + 0x12c))) > 0) {
            DamageNumberDisplay_Show(collider, *((s16 *)(p + 0x12c)), 0, p + 0x78, p + 0x80, 1);
            SceneHud_SetTimedState(*((void **)(p + 0x148)), *((s16 *)(p + 0x132)),
                                   *((s16 *)(p + 0x136)), *((s16 *)(p + 0x13c)), p[0xaf] == 4,
                                   *((s16 *)(p + 0x144)));
        }
        if ((*((s16 *)(p + 0x132))) <= 0) {
            const u16 *table;
            s32 count;
            *((s16 *)(p + 0x130)) = 1;
            *((s16 *)(p + 0xb0)) = 0;
            CopyPosition(&position, (struct SceneVec *)(p + 0x40));
            {
                struct SceneVec *v = &position;
                v->y += 200;
            }
            velocity.x = -4;
            table = (const u16 *)0x0203B400;
            velocity.y = RandomFrom(table) & 7;
            velocity.z = -4;
            size.x = 24;
            size.y = (RandomFrom(table) & 7) + 3;
            size.z = 24;
            count = (RandomFrom(table) & 3) + 4;
            sub_08014634(count, 2, &position, &velocity, &size, 15, 15);
            Anim_RestartSequence(p + 0x10c, *((void **)(p + 0x164)), 10, 0, 0);
            AnimData_SelectPaletteBank(p + 0x50, 0x1bc);
            *((s16 *)(p + 0x128)) = 0;
            *((void (**)(void))(p + 0x108)) = sub_082086B8;
            *((u32 *)(p + 0x18)) = 0;
            Sound_PlaySong(0x52c);
        }
        *((s16 *)(p + 0x128)) = 8;
        *((s16 *)(p + 0x12e)) = 1;
        *((s16 *)(p + 0x12c)) = 0;
    }
    if (((Mod(*((u32 *)(p + 0x18)), 240) == 0) && ((*((s16 *)(p + 0x130))) == 0)) &&
        ((*((void (**)(void))(p + 0x108))) != CannonTurret_FireAimedVolley)) {
        Anim_SetSequence(p + 0x10c, *((void **)(p + 0x164)), 9, 0, 8);
        Anim_SetDurationScale(p + 0x10c, 32);
        *((s16 *)(p + 0x134)) = 1;
    }
    if ((*((s16 *)(p + 0x128))) > 0) {
        AnimData_SelectPaletteBank(p + 0x50, 0x192);
        (*((s16 *)(p + 0x128)))--;
    } else
        AnimData_SelectPaletteBank(p + 0x50, 0x1bc);
    if (*((u16 *)(p + 0xb0))) {
        (*((u16 *)(p + 0xb0)))--;
        if (((*((u16 *)(p + 0xb0))) >> 2) & 1)
            *((u32 *)(p + 0x24)) |= 1;
        else
            *((u32 *)(p + 0x24)) &= ~1;
    }
    token = 0;
    asm("" : "+r"(token));
    ended = StepAnimation_UpdateCombat(p, (void *)(p + 0x10c), 4);
    asm("" : : "r"(token));
    if ((ended && ((*((s16 *)(p + 0x134))) != 0)) && ((*((s16 *)(p + 0x130))) == 0)) {
        Anim_SetSequence(p + 0x10c, *((void **)(p + 0x164)), 8, 0, 0);
        *((s16 *)(p + 0x134)) = 0;
    }
    (*((u32 *)(p + 0x18)))++;
    {
        register s32 result asm("r0") = 0;
        asm("" : "+r"(result));
        return result;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", CannonTurret_UpdateCombat);
#endif
