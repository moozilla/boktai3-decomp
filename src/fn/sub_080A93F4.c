#include "global.h"
#include "tentacle_segment_animation.h"
#include "display_animation.h"
#include "display_sequence_inline.h"
// MATCHING_CONCESSION: register-pinning

void *SceneActorProfiles_GetEntryAddress(u32, u32, s32);
void TentacleBody_GetDirectionFrame(u16, s32 *, s32 *);
void Anim_RestartSequence(void *, void *, u32, u8, u8);
void Anim_SetDurationScale(void *, u32);
static inline u8 TestMask4(struct TentacleSegmentAnimation *p, u32 mask)
{
    register u32 m asm("r4");
    u32 f = p->status;
    m = mask;
    if (f & m)
        return 1;
    return 0;
}
static inline u8 TestMask(struct TentacleSegmentAnimation *p, u32 mask)
{
    u32 f = p->status;
    if (f & mask)
        return 1;
    return 0;
}
void TentacleBody_RestartSegmentAnimations(struct TentacleBodyAnimationView *input, s32 kindIn,
                                           s32 rowIn, s32 flagsIn)
{
    struct TentacleBodyAnimationView *body = input;
    struct {
        s32 kind, flags;
        u8 *actor, *profile;
        s32 i;
    } frame;
    s32 row;
    register s32 sourceRow asm("r2") = rowIn;
    s32 offset;
    struct TentacleSegmentAnimation *a;
    register struct TentacleAnimationSegment *s asm("r8");
    frame.kind = kindIn;
    row = sourceRow;
    frame.flags = flagsIn;
    frame.actor = (void *)body;
    frame.profile = SceneActorProfiles_GetEntryAddress(body->profileId, body->profileMode, 0);
    frame.i = 0;
    if (frame.i >= body->segmentCount)
        return;
    offset = 0;
    a = &body->segments[0].animation;
    {
        u32 t = 0x3d4;
        t += (u32)body;
        s = (void *)t;
    }
    do {
        if (!TestMask4(a, 1)) {
            if (TentacleSegment_HasStatus(a, 4)) {
                Anim_RestartSequence(a, TentacleSegment_ScriptAt(body, offset), 1, row, ({
                                         s32 t = frame.flags;
                                         u32 u = (u8)t;
                                         asm("" ::"r"(t));
                                         u;
                                     }));
            } else {
                s32 four = 4;
                Anim_RestartSequence(a, TentacleSegment_ScriptAt(body, offset),
                                     *(u8 *)(({
                                                 register s32 k asm("r2") = frame.kind;
                                                 register s32 x asm("r0") = k * four;
                                                 x;
                                             }) +
                                             (u32)({
                                                 register u8 *pr asm("r3") = frame.profile;
                                                 pr;
                                             })),
                                     row, ({
                                         register s32 t asm("r4") = frame.flags;
                                         u32 u = (u8)t;
                                         asm("" ::"r"(t));
                                         u;
                                     }));
                {
                    register u8 *act asm("r1") = frame.actor;
                    register u16 *ad asm("r0");
                    ad = &((struct TentacleBodyAnimationView *)act)->speed;
                    Anim_SetDurationScale(a, *ad);
                }
            }
            {
                register struct DisplaySequenceSprite *other asm("r6") = (void *)s;
                struct DisplaySequenceState *state = &a->sequence;
                const u16 *command;
                u32 ended;
                command = state->commands + state->index;
                other->frame = *command >> 6;
                {
                    union DisplaySequenceControl control;
                    u32 bit, first;
                    control.raw = state->flags;
                    bit = 1;
                    first = (u8)(control.raw & bit);
                    if (first != (((*command & 0x30) >> 4) & ({
                                      register u32 m3 = 1;
                                      m3;
                                  })))
                        other->flags |= 4;
                    else {
                        register u32 *readFlags asm("r1") = &s->flags;
                        register u32 *writeFlags asm("r2");
                        register u32 value asm("r0") = *readFlags;
                        value &= ~4;
                        writeFlags = &s->flags;
                        *writeFlags = value;
                    }
                }
                {
                    union DisplaySequenceControl control;
                    u32 bit, first;
                    control.raw = state->flags;
                    bit = 2;
                    first = (u8)(control.raw & bit);
                    if (first != (((*command & 0x30) >> 4) & bit))
                        other->flags |= 8;
                    else
                        other->flags &= ~8;
                }
                {
                    u32 elapsed = state->elapsed + 1;
                    u32 mask;
                    register u32 temp asm("r3");
                    u32 savedDuration;
                    register u32 duration asm("r3");
                    u32 flags;
                    state->elapsed = elapsed;
                    temp = 0xffff;
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
                                u32 f8 = 8;
                                f8 &= flags;
                                if (f8)
                                    state->elapsed = savedDuration;
                                else
                                    state->index = state->frameCount - 1;
                            } else
                                state->index = index - 1;
                        } else {
                            u32 index = state->index + 1;
                            u32 frameCount;
                            state->index = index;
                            frameCount = state->frameCount;
                            if ((index & mask) >= frameCount) {
                                u32 endFlag = flags & 8;
                                asm("" : "+r"(endFlag));
                                if ((u8)endFlag) {
                                    state->elapsed = duration;
                                    state->index = frameCount - 1;
                                } else
                                    state->index = 0;
                            }
                        }
                        command = state->commands + state->index;
                        {
                            u32 sample = DisplaySequence_ReadCommand(command);
                            u32 mask = 15;
                            mask &= sample;
                            state->frameWeight = mask;
                        }
                        {
                            u32 r = (state->frameWeight * state->speed) >> 6;
                            state->duration = r;
                            if (!(u8)r) {
                                u32 one = 1;
                                state->duration = one;
                            }
                        }
                    }
                }
            }
        }
        {
            register u32 step asm("r1") = sizeof(struct TentacleAnimationSegment);
            offset += step;
            a = (void *)((u8 *)a + step);
            s = (void *)((u8 *)s + step);
        }
    } while (({
                 register s32 t asm("r2") = frame.i + 1;
                 frame.i = t;
                 t;
             }) < ({
                 register u32 d asm("r1");
                 s32 cnt;
                 cnt = body->segmentCount;
                 asm("" ::"r"(d));
                 cnt;
             }));
}
