// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "actor_attachment_state.h"
#include "background_controller.h"
struct FixedPosition {
    s32 x, y, z;
};
struct MarkerSlot {
    u8 resource[0x1c];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    const struct FixedPosition *anchor;
    u32 active;
};
struct MarkerPool {
    u8 task[0x18];
    struct MarkerSlot slots[8];
};
struct MarkerCursor {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    const struct FixedPosition *anchor;
    u32 active;
    u8 nextResource[0x1c];
};
struct MarkerTail {
    u16 y;
    u8 unknown2[0x10];
    u8 flags;
};
struct Camera {
    u8 unknown0[0x44];
    s32 x;
};
s32 ActorMarkerPool_UpdateMarkers(struct MarkerPool *pool)
{
    struct MarkerPool *base = pool;
    struct MarkerCursor *cursor = (void *)&base->slots[0].sprite;
    struct DisplaySequenceState *sequence = &base->slots[0].sequence;
    struct ContactSwarmSprite *sprites = &cursor->sprite;
    struct MarkerTail *tail = (void *)&base->slots[0].sprite.position.y;
    register s32 index asm("r10");
    s32 remaining;
    {
        register s32 zero asm("r2") = 0;
        asm("" : "+r"(zero));
        index = zero;
    }
    remaining = 7;
    do {
        u8 *active = (u8 *)&base->slots[0].active;
        if (*(u32 *)(active + index)) {
            const struct FixedPosition **anchorSlot = &base->slots[0].anchor;
            const struct FixedPosition *anchor;
            struct ContactSwarmSprite *sprite;
            struct DisplaySequenceState *state;
            const volatile u16 *command;
            u32 duration, flags;
            anchorSlot = (void *)((u8 *)anchorSlot + index);
            {
                const struct FixedPosition *read = *anchorSlot;
                anchor = read;
            }
            cursor->sprite.position.x = (anchor->x >> 12) - gSceneCamera->x;
            anchor = *anchorSlot;
            cursor->sprite.position.y = ((anchor->z + anchor->y) >> 12) + 0x5c;
            sprite = sprites;
            state = sequence;
            {
                u32 offset = cursor->sequence.index << 1;
                const volatile u16 *table = cursor->sequence.commands;
                command = (void *)((u32)table + offset);
            }
            cursor->sprite.frame = *command >> 6;
            {
                struct MarkerTail *read = tail;
                u32 input = read->flags;
                u32 first = 1;
                first &= input;
                if (first != (((*command & 0x30) >> 4) & 1))
                    cursor->sprite.flags |= 4;
                else
                    sprites->flags &= ~4;
            }
            {
                register u32 input asm("r0") = state->flags;
                register u32 first asm("r1");
                first = 2 & input;
                first = (u8)first;
                {
                    u32 encoded = (*command & 0x30) >> 4;
                    u32 bit = 2;
                    asm volatile("" : "+r"(bit) : "r"(encoded));
                    encoded &= bit;
                    if (first != encoded)
                        sprite->flags |= 8;
                    else
                        sprite->flags &= ~8;
                }
            }
            {
                u32 elapsed = state->elapsed + 1;
                u32 mask;
                register u32 temp asm("r2");
                register u32 savedDuration asm("r2");
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
                        u32 frame = state->index;
                        if (!frame) {
                            u32 hold = flags & 8;
                            asm("" : "+r"(hold));
                            if (hold)
                                state->elapsed = savedDuration;
                            else
                                state->index = state->frameCount - 1;
                        } else
                            state->index = frame - 1;
                    } else {
                        u32 frame = state->index + 1;
                        u32 count;
                        state->index = frame;
                        count = state->frameCount;
                        if ((frame & mask) >= count) {
                            u32 endFlag = flags & 8;
                            asm("" : "+r"(endFlag));
                            endFlag = (u8)endFlag;
                            if (endFlag) {
                                state->elapsed = duration;
                                state->index = count - 1;
                            } else
                                state->index = 0;
                        }
                    }
                    {
                        u32 offset = state->index << 1;
                        const volatile u16 *table = state->commands;
                        command = (void *)((u32)table + offset);
                    }
                    state->frameWeight = *command & 15;
                    {
                        s32 speed = (state->frameWeight * state->speed) >> 6;
                        state->duration = speed;
                        if (!(u8)speed)
                            state->duration = 1;
                    }
                }
            }
        }
        cursor++;
        {
            register u32 step asm("r1") = 0x60;
            sequence = (void *)((u8 *)sequence + step);
            sprites = (void *)((u8 *)sprites + 0x60);
            tail = (void *)((u8 *)tail + step);
            index += step;
        }
        {
            register s32 next asm("r2") = remaining - 1;
            remaining = next;
            if (next < 0)
                break;
        }
    } while (1);
    return 0;
}
