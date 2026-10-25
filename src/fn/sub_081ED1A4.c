#include "global.h"

#include "contact_swarm.h"

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
        u32 first;
        u32 maskBit = 1;
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
        u32 bit;
        u32 first;
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
        u32 temp;
        u32 savedDuration;
        state->elapsed = elapsed;
        temp = 0xFFFF;
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */
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
                s32 weight = state->frameWeight;
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

void ActorDisplay_UpdateScreenAnchor(void *);
void ActorDisplay_UpdateForegroundSprite(void *);
void ActorDisplay_UpdateAccentSprite(void *);
void ActorMotion_AdvanceTowardScreenTarget(void *, s32);
void SceneActors_EmitPeriodicParticles(void *, s32);
extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
extern u8 *gUnk_020004F0;
extern u8 *gUnk_020004B8;
s32 Player_GetHorizontalVelocity(s32);
#define W(o) (*(s32 *)(p + (o)))
#define H(o) (*(u16 *)(p + (o)))
#define S(o) (*(s16 *)(p + (o)))
#ifdef NONMATCHING
// fuzzy: 54.1%
// fix.py best (sweep after the inlined-insn fix)
// confidence=low: two animated layers track the active view with a velocity state machine.
#define SceneActors_UpdateCameraFollowingDisplay sub_081ED1A4
static inline u32 StepPlayback_081ED1A4(struct Drawable *owner, struct DisplaySequenceState *state)
{
    struct ContactSwarmSprite *sprite = &owner->sprite;
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
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        u32 first;
        u32 maskBit = 1;
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
        u32 bit;
        u32 first;
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
        u32 temp;
        u32 savedDuration;
        state->elapsed = elapsed;
        temp = 0xFFFF;
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */
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
                s32 weight = state->frameWeight;
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

void SceneActors_UpdateCameraFollowingDisplay(u8 *input, s32 time)
{
    u8 *p = input;
    s32 savedTime;
    u8 *context;
    u8 *enter;
    savedTime = time;
    context = p;
    enter = p + 0xB1;
    if (*enter) {
        struct Drawable *foreground;
        struct Drawable *accent;
        struct DisplaySequenceState *state;
        u32 zero;
        zero = 0;
        *enter = zero;
        foreground = (struct Drawable *)(p + 0x2C0);
        accent = (struct Drawable *)(p + 0x308);
        *((u32 *)(p + 0x298)) = zero;
        *((u32 *)(p + 0x290)) = zero;
        foreground->unknown3C = zero;
        state = (struct DisplaySequenceState *)(p + 0x2EC);
        Anim_SetSequence(state, *((void **)(p + 0x390)), zero, 0, zero);
        StepPlayback_081ED1A4(foreground, state);
        accent->unknown3C = 4;
        state = &accent->sequence;
        Anim_SetSequence(state, *((void **)(context + 0x390)), 4, 0, 0);
        StepPlayback_081ED1A4(accent, state);
    }
    {
        struct CameraVector {
            s32 x;
            s32 y;
            s32 z;
        } camera;
        s32 distance;
        s32 speed;
        if (!gUnk_020004F0) {
            camera.x = (camera.y = (camera.z = 0));
        } else {
            camera = *((struct CameraVector *)(gUnk_020004F0 + 0x28));
        }
        distance = (camera.x - (*((s32 *)(p + 0x268)))) >> 12;
        speed = Player_GetHorizontalVelocity(*((s32 *)(p + 0x3A0)));
        gUnk_03005304 = (gUnk_03005304 + 1) & 1023;
        {
            u32 value = gUnk_086141BC[gUnk_03005304];
            u32 *offset = (u32 *)(context + 0x288);
            *offset = value & 1;
        }
        if (distance < 0) {
            *((s32 *)(p + 0x284)) = -128;
            *((s32 *)(p + 0x398)) = 1;
        } else if (distance <= 63) {
            if (((u32)((*((s32 *)(p + 0x398))) - 2)) > 1)
                *((s32 *)(p + 0x398)) = (speed > (*((s32 *)(p + 0x280)))) ? (2) : (3);
        } else if (distance > 64) {
            *((s32 *)(p + 0x284)) = 128;
            *((s32 *)(p + 0x398)) = 0;
        }
        switch (*((s32 *)(p + 0x398))) {
            case 0:

            case 1:
                *((s32 *)(p + 0x280)) += *((s32 *)(p + 0x284));
                if ((*((s32 *)(p + 0x280))) > 0x6000)
                    *((s32 *)(p + 0x280)) = 0x6000;
                if ((*((s32 *)(p + 0x280))) <= 0x1FFF)
                    *((s32 *)(p + 0x280)) = 0x2000;
                break;

            case 2:
                *((s32 *)(p + 0x284)) = 128;
                *((s32 *)(p + 0x280)) += 128;
                if ((*((s32 *)(p + 0x280))) > speed) {
                    *((s32 *)(p + 0x280)) = speed;
                    *((s32 *)(p + 0x284)) = 0;
                }
                break;

            case 3:
                *((s32 *)(p + 0x284)) = -128;
                *((s32 *)(p + 0x280)) -= 128;
                if ((*((s32 *)(p + 0x280))) < speed) {
                    {
                        typeof(*((s32 *)(p + 0x280))) *q1 = &(*((s32 *)(p + 0x280)));
                        u8 z1 = 4;
                        *q1 = speed;
                        *((s32 *)(p + 0x284)) = 0;
                        *((s32 *)(p + 0x398)) = z1;
                    }
                }
                break;
        }

        *((s32 *)(p + 0x268)) += *((s32 *)(p + 0x280));
        *((s32 *)(p + 0x270)) = *((s32 *)(p + 0x2AC));
        *((u16 *)(p + 0x278)) =
            ((*((s32 *)(p + 0x268))) >> 12) - (*((s32 *)(gUnk_020004B8 + 0x44)));
        *((u16 *)(p + 0x27A)) = (((*((s32 *)(p + 0x26C))) + (*((s32 *)(p + 0x270)))) >> 12) + 96;
        *((u16 *)(p + 0x27C)) = (*((s32 *)(p + 0x270))) >> 12;
        if ((*((s16 *)(p + 0x278))) > 160) {
            *((s32 *)(p + 0x268)) -= *((s32 *)(p + 0x280));
            *((s32 *)(p + 0x280)) = 0;
        }
    }
    ActorDisplay_UpdateScreenAnchor(p);
    ActorDisplay_UpdateForegroundSprite(p);
    ActorDisplay_UpdateAccentSprite(p);
    SceneActors_EmitPeriodicParticles(p, savedTime);
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081ED1A4);
#endif
