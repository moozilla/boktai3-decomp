// MATCHING_CONCESSION: register-pinning
#include "display_animation.h"
#include "background_controller.h"
// Updates a dropped sprite only when its manager requests an animation tick.
struct Render {
    u32 flags;
    u8 pad4[12];
    u16 frame;
};
struct Drop {
    u8 pad0[0x1C];
    struct Render render;
    u8 pad30[0x38 - 0x30];
    s16 screenX, screenY, screenZ;
    u8 pad3E[0x48 - 0x3E];
    struct DisplaySequenceState sequence;
    u8 pad58[0x9C - 0x58];
    s16 x, y, z;
    u8 padA2[0xB1 - 0xA2];
    u8 tick;
};
void SpatialIndex_SetPosition(void *, s32, s32, s32);
void SpatialIndex_InsertEntry(void *);
u8 SpriteDrop_IsNearPlayer(void *);
static inline void SetDropPosition(void *p, s32 x, s32 y, s32 z)
{
    SpatialIndex_SetPosition(p, x, y - z, z);
}
void SpriteDrop_UpdateEntry(struct Drop *p)
{
    struct DisplaySequenceState *state;
    const volatile u16 *command;
    u32 duration, flags, bit;
    struct Render *render;
    if (p->tick) {
        p->screenY = p->y - *(s32 *)(((u8 *)gSceneCamera) + 0x48);
        p->screenZ = p->z;
        p->render.flags &= ~1;
        {
            void *entry = (u8 *)p + 0x5C;
            s32 x = p->x;
            s32 y = p->y;
            s32 z;
            y -= 96;
            z = p->z;
            SpatialIndex_SetPosition(entry, x, y - z, z);
        }
        render = &p->render;
        state = &p->sequence;
        {
            u32 offset = state->index << 1;
            const volatile u16 *table;
            table = p->sequence.commands;
            command = (const volatile u16 *)((u32)table + offset);
        }
        render->frame = *command >> 6;
        {
            u32 input = state->flags;
            register u32 first asm("r1");
            register u32 maskBit asm("r3") = 1;
            bit = maskBit;
            first = bit;
            first &= input;
            if (first != (((*command & 0x30) >> 4) & bit))
                p->render.flags |= 4;
            else
                p->render.flags &= ~4;
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
                render->flags |= 8;
            else
                render->flags &= ~8;
        }
        {
            u32 elapsed = state->elapsed + 1;
            register u32 mask asm("r6");
            register u32 temp asm("r1");
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
            }
        }

        p->tick = 0;
    }
    p->screenX = p->x - *(s32 *)(((u8 *)gSceneCamera) + 0x44);
    if (SpriteDrop_IsNearPlayer(p))
        SpatialIndex_InsertEntry((u8 *)p + 0x5C);
}
