// MATCHING_CONCESSION: register-pinning
#include "global.h"

// Remaining differences: playback pointer lifetimes and a few operand/order choices.

#include "contact_swarm.h"
#include "player_cue_sequence.h"

// Drawable layout shared with ActorDisplay_UpdateForegroundSprite.
struct Drawable {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    u32 unknown3C;
    s32 ended;
    u32 mode;
};
static inline u32 StepPlayback(struct Drawable *owner, struct DisplaySequenceState *state)
{
    return PlayerCue_StepSequenceEnded(&owner->sprite, state);
}

static inline u32 StepPlaybackOne(struct Drawable *owner, struct DisplaySequenceState *state,
                                  u32 maskBit)
{
    struct ContactSwarmSprite *sprite = &owner->sprite;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 4;
        else
            owner->sprite.flags &= ~4;
    }
    {
        u32 input = state->flags;
        register u32 bit asm("r3");
        register u32 first asm("r1");
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 8;
        else
            owner->sprite.flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
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
                register s32 weight asm("r1") = state->frameWeight;
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

static inline u32 ConsumeEntry(u8 *p)
{
    u8 *q = p + 0xB0;
    register u32 result asm("r1");
    if (*q) {
        *q = 0;
        *(q - 1) = 0;
        result = TRUE;
    } else
        result = FALSE;
    return result;
}

void Anim_RestartSequence(struct DisplaySequenceState *, void *, u32, u32, u32);
void SceneMover_InterpolateScreenX(void);

static inline void SetCallback(void (*v)(void), void (**a)(void))
{
    *a = v;
}
#define SETCB(p)                                                                                   \
    do {                                                                                           \
        SetCallback(SceneMover_InterpolateScreenX, (void (**)(void))((p) + 0x100));                \
        *(u32 *)((p) + 0x90) = 0;                                                                  \
        *((p) + 0xB1) = 1;                                                                         \
        *((p) + 0xA2) = 0;                                                                         \
        *(u32 *)((p) + 0xA4) = 0;                                                                  \
    } while (0)

void SingletonSlot7_UpdateEndingScene(u8 *p)
{
    u8 *context = p;
    u32 flag = ConsumeEntry(context);
    if (flag == 1) {
        struct Drawable *foreground = (struct Drawable *)(p + 0x2C0);
        struct DisplaySequenceState *state;
        *(u32 *)(p + 0x298) = 0;
        *(u32 *)(p + 0x290) = 0;
        *(u32 *)(p + 0x294) = 0x28;
        *(u16 *)(p + 0x5D2) = 0x3C;
        foreground->unknown3C = 2;
        state = (struct DisplaySequenceState *)(p + 0x2EC);
        Anim_SetSequence(state, *(void **)(p + 0x390), 2, 0, 0);
        asm("" : "+r"(flag));
        StepPlaybackOne(foreground, state, flag);
        SETCB(context);
        *(u32 *)(p + 0x39C) = 0;
    }
    switch (*(u32 *)(p + 0x39C)) {
        case 0:
            if (*(context + 0xAF)) {
                struct Drawable *foreground;
                struct DisplaySequenceState *state;
                u32 *field = (void *)(p + 0x304);
                register u32 one asm("r5") = 1;
                *field = one;
                *(u32 *)(p + 0x2FC) = 3;
                state = (struct DisplaySequenceState *)(p + 0x2EC);
                Anim_SetSequence(state, *(void **)(p + 0x390), 3, 0, 0);
                foreground = (struct Drawable *)(p + 0x2C0);
                StepPlaybackOne(foreground, state, one);
                *(u32 *)(p + 0x39C) = 1;
                *(u16 *)(p + 0x5DE) = 0;
            }
            break;
        case 1:
            if (*(s16 *)(p + 0x5DE) > 0x78) {
                u32 *field = (void *)(p + 0x304);
                u32 zero = 0;
                *field = zero;
                *(u32 *)(p + 0x294) = 0xA0;
                {
                    u16 *timer = (void *)(p + 0x5D2);
                    register u8 byteZero asm("r3") = 0;
                    void (*callback)(void);
                    u32 offset = 0x1E;
                    *timer = offset;
                    callback = SceneMover_InterpolateScreenX;
                    offset += 0xE2;
                    SetCallback(callback, (void (**)(void))(context + offset));
                    *(u32 *)(context + 0x90) = zero;
                    context[0xB1] = 1;
                    context[0xA2] = byteZero;
                    *(u32 *)(context + 0xA4) = zero;
                }
                *(u32 *)(p + 0x39C) = 2;
            }
            (*(u16 *)(p + 0x5DE))++;
            break;
        case 2:
            if (*(context + 0xAF)) {
                *(u32 *)(p + 0x39C) = 3;
                *(u16 *)(p + 0x5DE) = 0;
            }
            break;
        case 3:
            if (*(s16 *)(p + 0x5DE) > 0x78) {
                u32 *field = (void *)(p + 0x304);
                u32 zero = 0;
                struct DisplaySequenceState *state;
                *field = zero;
                *(u32 *)(p + 0x2FC) = 3;
                {
                    u8 *field = p + 0x2F0;
                    u32 v = *field;
                    u32 mask = 4;
                    asm("" : "+r"(mask));
                    *field = mask | v;
                }
                state = (struct DisplaySequenceState *)(p + 0x2EC);
                Anim_RestartSequence(state, *(void **)(p + 0x390), *(u16 *)(p + 0x2FC), 0, zero);
                *(u32 *)(p + 0x294) = 0x3C;
                {
                    u16 *timer = (void *)(p + 0x5D2);
                    register u8 byteZero asm("r2") = 0;
                    register void (*callback)(void);
                    u32 offset = 0xB4;
                    *timer = offset;
                    callback = SceneMover_InterpolateScreenX;
                    offset += 0x4C;
                    SetCallback(callback, (void (**)(void))(context + offset));
                    *(u32 *)(context + 0x90) = zero;
                    context[0xB1] = 1;
                    context[0xA2] = byteZero;
                    *(u32 *)(context + 0xA4) = zero;
                }
                *(u32 *)(p + 0x39C) = 4;
            }
            (*(u16 *)(p + 0x5DE))++;
            break;
        case 4:
            if (*(context + 0xAF)) {
                struct Drawable *foreground;
                struct DisplaySequenceState *state;
                u32 *field = (void *)(p + 0x5BC);
                register u32 one asm("r5") = 1;
                *field = one;
                *(u32 *)(p + 0x2FC) = 0;
                state = (struct DisplaySequenceState *)(p + 0x2EC);
                Anim_SetSequence(state, *(void **)(p + 0x390), 0, 0, 0);
                foreground = (struct Drawable *)(p + 0x2C0);
                StepPlaybackOne(foreground, state, one);
                Trigger_ConsumePending(context + 0x118, 1);
            }
            break;
    }
    (*(u32 *)(context + 0xC4))++;
}
