#include "global.h"
#include "render.h"
#include "display_sequence.h"
#ifdef NONMATCHING
// fuzzy: 95.8%
struct Vec {
    s16 x, y, z, pad;
};
struct FragmentSprite {
    u32 flags;
    u8 unknown4[0xc];
    u16 frame;
    u8 unknown12[0xa];
    struct Vec position;
    u8 unknown24[8];
};
struct FragmentBurst {
    struct FragmentSprite sprites[4];
    struct DisplaySequenceState sequences[4];
    struct Vec origin, position;
    u8 collider[0x54];
    u16 age;
    u8 state;
    s8 slot;
    u32 completionScript;
    u16 timer;
    u8 enter;
};
void Sound_PlaySong(u32);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);
static inline s32 ProjectScale(s32 n)
{
    s32 v = n * 3;
    if (v >= 0)
        v >>= 5;
    else
        v = -(-v >> 5);
    return v;
}
static inline s32 ProjectHeight(s32 n)
{
    s32 v = n * 3;
    if (v >= 0)
        return v >> 5;
    return -(-v >> 5);
}
static inline void Project(struct Vec *out, struct Vec *in)
{
    s32 x = in->x, z = in->z, h, y;
    out->x = ProjectScale(x - z);
    h = ProjectScale(x + z);
    y = ProjectHeight(in->y);
    out->y = h - y;
    out->z = h + y;
}
// confidence=low: animates three projected burst sprites and enables their short-lived impact collider.
static inline void ProjectShift(struct Vec *out, struct Vec *in, struct Vec *camera)
{
    Project(out, in);
    out->x = out->x - camera->x + 120;
    out->y = out->y - camera->y + 80;
    out->z -= camera->z;
}
static inline u16 DisplaySequence_GetIndex(struct DisplaySequenceState *state)
{
    return state->index;
}

static inline u32 DisplaySequence_ReadCommand(const u16 *command)
{
    return *command;
}

static inline u32 DisplaySequence_MaskCommand(u32 command, u32 mask)
{
    mask &= command;
    return mask;
}

/* Apply the current frame and flips, then advance the stored playback state.
 * The duration calculation uses the newly selected command's low four bits. */
static inline void FragmentBurst_StepAnimation(struct FragmentSprite *owner,
                                               struct DisplaySequenceState *state, u32 endMask)
{
    const u16 *command;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    owner->frame = *command >> 6;
    {
        u32 input = state->flags;
        u32 first = 1;
        u8 bit = 1;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 4;
        else
            owner->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= endMask;
        else
            owner->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        u32 flags;
        state->elapsed = 0;
        flags = state->flags;
        if (flags & 4) {
            if (state->index == 0) {
                if (flags & endMask)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
            } else {
                state->index--;
            }
        } else {
            if (++state->index >= state->frameCount) {
                u32 hold = endMask;
                hold &= flags;
                if ((u8)hold) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    }
}

void FragmentBurst_UpdateImpactAnimation(void *unused, struct FragmentBurst *p)
{
    s32 i;
    struct Vec *camera;
    u32 endMask;
    if (p->enter) {
        Sound_PlaySong(0x3B1);
        p->enter = 0;
    }
    i = 0;
    do {
        struct FragmentSprite *sprite = &p->sprites[i];
        endMask = 8;
        camera = &gCameraOrigin;
        sprite->flags &= ~1;
        FragmentBurst_StepAnimation(sprite, &p->sequences[i], endMask);
        ProjectShift(&sprite->position, &p->origin, camera);
        sprite->position.y += i * 32;
        i++;
    } while (i < 3);
    if (p->timer > 4) {
        p->state = 2;
        p->enter = 1;
    } else {
        Collider_SetPosition(p->collider, &p->position, 0);
        Collider_Refresh(p->collider);
        p->timer++;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", FragmentBurst_UpdateImpactAnimation);
#endif
