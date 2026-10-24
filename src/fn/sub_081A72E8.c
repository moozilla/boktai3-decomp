// MATCHING_CONCESSION: register-pinning
#include "route_sprite_playback.h"
void Vanargand_UpdatePartCollision(void *boss, void *part);
void sub_081A7204(void *part);
void SceneAnimation_SelectSequence(void *, s32, s32, s32);
void Vanargand_UpdatePartAnimation(void *boss, struct RouteSpriteActor *part)
{
    u32 f = part->sprite.flags;
    u32 one = 1;
    asm("" : "+r"(one));
    if (!(f & one)) {
        struct DisplaySequenceState *state;
        const volatile u16 *command;
        u32 ended, duration, flags;
        *(u32 *)((u8 *)part + 0x100) = 0;
        sub_081A7204(part);
        if (*(u16 *)((u8 *)part + 0x104)) {
            state = &part->sequence;
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            part->sprite.frame = *command >> 6;
            {
                u32 input = state->flags;
                register u32 first asm("r1");
                first = one;
                first &= input;
                if (first != (((*command & 0x30) >> 4) & one))
                    part->sprite.flags |= 4;
                else
                    part->sprite.flags &= ~4;
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
                if (first != (((*command & 0x30) >> 4) & bit))
                    part->sprite.flags |= 8;
                else
                    part->sprite.flags &= ~8;
            }
            {
                u32 elapsed = state->elapsed + 1;
                u32 mask;
                register u32 temp asm("r2");
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
            if (ended) {
                {
                    register u32 off asm("r1") = 0x104;
                    register u16 *q asm("r0");
                    asm("" : "+r"(off));
                    q = (u16 *)((u8 *)part + off);
                    *q = 0;
                }
                if (*(u32 *)((u8 *)part + 0x5c) == 2)
                    SceneAnimation_SelectSequence(part, 3, 0, 0);
            }
            *(u32 *)((u8 *)part + 0x100) |= 1;
        }
        Vanargand_UpdatePartCollision(boss, part);
    }
}
