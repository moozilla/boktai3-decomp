// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: planar debris lifetime follows completion of its visual animation.
/* ROM copies an uninitialized projected depth halfword; keep it uninitialized to match. */
#include "actor_attachment_state.h"
#include "background_controller.h"
struct Visual {
    u8 unknown0[0x1c];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
};
struct VisualAnim {
    u8 unknown0[0x40];
    struct ContactPosition position;
};
struct Debris {
    struct Visual *visual;
    struct ContactPosition position, velocity;
    u32 timer;
    u8 restitution, active, entered, visualType;
    void (*update)(struct Debris *);
};
struct Camera {
    u8 unknown0[0x44];
    s32 x, y;
};
struct Packed {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
s32 Div(s32, s32);
s32 Terrain_GetCellClearance(void *);
void SceneDebris_OnMotionEnd(struct Debris *);
static inline u32 StepDebrisSprite(struct Visual *visual)
{
    register struct Visual *v asm("r12") = visual;
    struct ContactSwarmSprite *sprite = &v->sprite;
    struct DisplaySequenceState *state = &v->sequence;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        register struct Visual *read asm("r2") = v;
        table = read->sequence.commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        register u32 maskBit asm("r3") = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        {
            register u32 value asm("r0");
            if (first != (((*command & 0x30) >> 4) & bit)) {
                register struct Visual *read asm("r1") = v;
                value = read->sprite.flags | 4;
            } else {
                register struct Visual *read asm("r1") = v;
                value = read->sprite.flags & ~4;
            }
            {
                register struct Visual *out asm("r2") = v;
                out->sprite.flags = value;
            }
        }
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
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
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

void SceneDebris_UpdateAnimatedMotion(struct Debris *p)
{
    struct Packed projected;
    s32 ended;
    struct ContactPosition *output;
    if (p->entered) {
        u32 *flags;
        if (!p->visualType)
            flags = &p->visual->sprite.flags;
        else
            flags = (u32 *)((u8 *)p->visual + 0x28);
        {
            u32 mask = ~1;
            *flags &= mask;
        }
        p->entered = 0;
    }
    p->position.x = p->position.x + p->velocity.x;
    p->position.y = p->position.y + p->velocity.y;
    projected.x = p->position.x - gSceneCamera->x;
    projected.y = p->position.y - gSceneCamera->y;
    output = (void *)&projected;
    if (!p->visualType) {
        struct Visual *visual = p->visual;
        visual->sprite.position.x = ((struct ContactPosition *)&projected)->x;
        visual->sprite.position.y = output->y;
        visual->sprite.position.z = output->z;
    } else {
        struct VisualAnim *visual = (void *)p->visual;
        visual->position.x = ((struct ContactPosition *)&projected)->x;
        visual->position.y = output->y;
        visual->position.z = output->z;
    }
    if (!p->visualType) {
        register struct Visual *input asm("r2") = p->visual;
        asm("" : "+r"(input));
        ended = StepDebrisSprite(input);
    } else
        ended = Anim_Step((u8 *)p->visual + 0x20, p->visual);
    if (ended) {
        p->update = SceneDebris_OnMotionEnd;
        p->entered = 1;
    }
}
