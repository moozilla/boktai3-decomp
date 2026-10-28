#include "global.h"
#ifdef NONMATCHING
// fuzzy: 90.5%
// fix.py best version (reprinted C)
// confidence=low: provisional role inferred from state transitions and player-motion calls
// confidence=low: moves a paired half and holds/releases player position requests.
// MATCHING_CONCESSION: register-pinning
#include "display_animation.h"
#include "player.h"
// Remaining differences: Packed-animation allocation, request-call setup, and shared phase-entry tails differ.
// confidence=low: moves a paired half toward the player, then holds and releases the position request.
struct PairPosition {
    s16 x, y, z, pad;
};
struct PairHalf {
    u32 flags;
    u8 pad4[0x10 - 4];
    u16 frame;
    u8 pad12[0x1C - 0x12];
    struct PairPosition position;
    u8 pad24[0x48 - 0x24];
    struct DisplaySequenceState sequence;
    u8 track[0x90 - 0x58];
    struct PairPosition *origin;
    u8 pad94[8];
    struct PairPosition offset;
    u8 padA4[0x15C - 0xA4];
    u32 mode, controlFlags;
    u32 phase, timer;
    u8 state, previousState, entered, phaseEntered, finished;
};
struct PairOwner {
    u8 pad0[0x4D0];
    void *resource;
};
struct CapturePlayer {
    u8 pad0[0x20];
    u32 flags;
};
void MotionTrack_Initialize(void *, void *, void *, s32, s32);
void MotionTrack_Update(void *);
void Player_BeginPositionRequest(void *, void *, s32, s32, s32, s32, s32, s32, s32);
void Player_ApplyPositionRequest(void *, void *, s32, s32, s32, s32, s32, s32);
static inline void StepSequence(void *obj)
{
    struct PairHalf *owner = obj;
    struct DisplaySequenceState *state;
    const volatile u16 *command;
    u32 duration;
    u32 flags;
    u32 bit;
    state = &owner->sequence;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = owner->sequence.commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    owner->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        bit = 1;
        first = bit;
        first &= input;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            owner->flags |= 4;
        else
            owner->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            owner->flags |= 8;
        else
            owner->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        u32 temp;
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
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
                } else {
                    state->index = index - 1;
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
                } else {
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)(((u32)table) + offset);
            }
            state->frameWeight = (*command) & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!((u8)speed))
                    state->duration = 1;
            }
        }
    }
}

static inline void initVector(struct PairPosition *v, s32 x, s32 y, s32 z)
{
    s16 *dst = (s16 *)v;
    dst[0] = x;
    dst[1] = y;
    dst[2] = z;
}
static inline void copyPosition(struct PairPosition *dst, struct PairPosition *src)
{
    s16 *d = (s16 *)dst;
    s16 *s = (s16 *)src;
    d[0] = s[0];
    d[1] = s[1] - 128;
    d[2] = s[2];
}
static inline u8 trackFinished(struct PairHalf *half)
{
    return *(u8 *)((u8 *)half + 0x81) == 2;
}
static inline void setPhase(struct PairHalf *half, u32 phase)
{
    half->phase = phase;
    half->timer = 0;
    half->phaseEntered = 1;
}
void PairedSceneActor_UpdatePlayerCapture(struct PairOwner *owner, struct PairHalf *half)
{
    struct PairPosition target;
    struct PairPosition position;
    asm("" : "+r"(owner));
    {
        u32 mask = 12;
        half->controlFlags |= mask;
    }
    if (half->entered) {
        half->entered = 0;
        Anim_SetSequence(&half->sequence, owner->resource, 12, (u8)half->mode, 0);
        StepSequence(half);
        setPhase(half, 0);
    }
    switch (half->phase) {
        case 0:
            if (half->phaseEntered) {
                half->phaseEntered = 0;
                initVector(&target, 384, 0, 384);
                MotionTrack_Initialize(half->track, &half->offset, &target, 0, 4);
            }
            copyPosition(&position, &half->position);
            if (half->mode == 0) {
                if (!((*(struct CapturePlayer **)gPlayers)->flags & 0x40000))
                    Player_BeginPositionRequest((*(struct CapturePlayer **)gPlayers), &position, 0,
                                                0, 1, 0, 0, 0, 1);
                else
                    goto abortCapture;
            } else if (!((*(struct CapturePlayer **)gPlayers)->flags & 0x40000))
                Player_BeginPositionRequest((*(struct CapturePlayer **)gPlayers), &position, 0, 0,
                                            0, 0, 0, 0, 2);
            else
                goto abortCapture;
            goto advanceTrack;
        abortCapture:
            setPhase(half, 3);

            break;
        advanceTrack:
            MotionTrack_Update(half->track);

            if (trackFinished(half))
                setPhase(half, 1);
            break;

        case 1:
            if ((++half->timer) > 120) {
                Anim_SetSequence(&half->sequence, owner->resource, 9, (u8)half->mode, 0);
                StepSequence(half);
                setPhase(half, 2);
            }
            break;

        case 2:
            Player_ApplyPositionRequest((*(struct CapturePlayer **)gPlayers), &half->position, 80,
                                        32, 128, 0, 0, 0);
            Anim_SetSequence(&half->sequence, owner->resource, 9, (u8)half->mode, 0);
            StepSequence(half);
            setPhase(half, 3);
            break;

        case 3:
            half->finished = 1;
            break;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", PairedSceneActor_UpdatePlayerCapture);
#endif
