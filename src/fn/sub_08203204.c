// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "display_animation.h"
#include "scene_vec.h"
struct CannonEmitter {
    u8 pad0[0x18];
    struct SceneVec initialPosition;
    struct DisplaySequenceSprite sprite;
    u8 animationData[0x1c];
    u8 body[0x48];
    s16 flashPalette[16];
    void (*state)(void);
    struct DisplaySequenceState sequence;
    s32 rangeSquared;
    s16 effectX, effectY, effectZ, solid;
    s16 timer, flashTimer, variant, spread;
    void *parent;
    u8 padFC[4];
    void *resource;
};
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
void ProximitySequence_TestPlayerDistance(void);
static inline u32 CannonEmitter_StepAnimation(struct CannonEmitter *owner,
                                              struct DisplaySequenceState *stateArg, u32 flipMask)
{
    register struct DisplaySequenceSprite *sprite asm("ip") = &owner->sprite;
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
        asm("" : "+r"(sprite) : "r"(frame));
        sprite->frame = frame;
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
            u32 flags = owner->sprite.flags;
            register u32 flip asm("r3") = flipMask;
            asm("" : : "r"(flip));
            owner->sprite.flags = flags | flip;
        } else
            owner->sprite.flags &= ~4;
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
                register struct DisplaySequenceSprite *read asm("r2") = sprite;
                flags = read->flags | 8;
            } else {
                register struct DisplaySequenceSprite *read asm("r2") = sprite;
                flags = read->flags & ~8;
            }
            {
                register struct DisplaySequenceSprite *out asm("r3") = sprite;
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

extern u32 gUnk_030053F0, gUnk_02000560;
static inline u32 CannonEmitter_TestPause(u32 mask)
{
    return (gEngineFlags | gUnk_030053F0) & mask;
}
static inline u32 CannonEmitter_TestMode(u32 mask)
{
    return gUnk_02000560 & mask;
}
s32 CannonEmitter_UpdateAnimation(struct CannonEmitter *input)
{
    struct CannonEmitter *p = input;
    register s32 result asm("r0");
    register u32 flipMask asm("r8");
    if (CannonEmitter_TestPause(0x400))
        goto paused;
    flipMask = 4;
    if (CannonEmitter_TestMode(flipMask)) {
    paused:
        return 0;
    }
    result = Callback_InvokeActor(p, p->state);
    p->sprite.flags &= ~1;
    if (p->flashTimer > 0) {
        *(void **)((u8 *)p + 0x58) = p->flashPalette;
        p->flashTimer--;
    }
    CannonEmitter_StepAnimation(p, &p->sequence, flipMask);
    return result;
}
