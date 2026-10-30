#include "scene_actor_reset.h"
#include "contact_swarm.h"
#include "display_sequence_inline.h"

struct AxeAttackTrailContext {
    u8 unknown0[0xA38];
    union {
        struct DisplaySequenceSprite sprite;
        struct ContactSwarmSprite spatial;
    } draw;
    struct ContactSwarmGraphics graphics;
    struct DisplaySequenceState sequence;
    struct ContactCollider attack[3], body[3];
    struct ContactPosition origin;
    u8 unknownC90[0xCA7 - 0xC90];
    u8 shadowActive;
    u8 unknownCA8[4];
    u8 shadow[0x40];
    u8 unknownCEC[0xED7 - 0xCEC];
    u8 variant;
};
void Collider_SetBodyProperties(void *, u16, u32, u32);
void Collider_SetAttackProperties(void *, u16, u16, u32, u32, u16);
void Axe_LatchColliderContact(void);
void Axe_OnSecondaryColliderContact(void);
u32 Shadow_Initialize(void *, void *, s32);

static inline u32 Axe_StepSequence(struct DisplaySequenceSprite *owner,
                                   struct DisplaySequenceState *state)
{
    const u16 *command;
    u32 ended;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    owner->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
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
            owner->flags |= 8;
        else
            owner->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
                ended = 1;
            } else {
                state->index--;
                ended = 0;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
                ended = 1;
            } else {
                ended = 0;
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    } else {
        ended = 0;
    }
    return ended;
}

static inline u16 Axe_SequenceSelection(struct DisplaySequenceState *source)
{
    return source->unknownA;
}

void Axe_InitAttackTrail(struct SceneActorRegistryResetView *s)
{
    struct ContactPackedVector bodySize;
    struct ContactPackedVector bodyOffset;
    struct ContactPackedVector attackSize;
    struct ContactPackedVector attackOffset;
    struct AxeAttackTrailContext *d = s->context;
    void *resource = &d->graphics;
    struct DisplaySequenceSprite *sprite;
    struct DisplaySequenceState *frames;
    void *sequence;
    s32 i;
    AnimData_Load(resource, 0x5291);
    sprite = &d->draw.sprite;
    Sprite_Init(sprite, resource, 0);
    sequence = ScriptProc_Start(0x922e, 0x3f72);
    {
        struct DisplaySequenceState *drawable =
            &((struct SceneActorControlMotionDrawable *)s->drawable)->sequence;
        u32 index = Axe_SequenceSelection(drawable);
        frames = &d->sequence;
        Anim_SetSequence(frames, sequence, index, d->variant, 0);
    }
    Axe_StepSequence(sprite, frames);
    d->draw.sprite.flags |= 1;
    d->draw.spatial.position = s->body.position;
    d->draw.sprite.priority = 1;
    bodySize.x = 24;
    bodySize.y = 70;
    bodySize.z = 24;
    bodyOffset.x = 0;
    bodyOffset.y = 0;
    bodyOffset.z = 0;
    for (i = 0; i < 3; i++) {
        void *c = &d->body[i];
        Collider_Init(c, 0, 0x5001, 0, 16, &bodySize, &bodyOffset);
        Collider_SetBodyProperties(c, 0, 0, 0);
        Collider_SetPosition(c, &d->origin, 0);
        Collider_SetCallback(c, Axe_LatchColliderContact, d);
        Collider_Refresh(c);
    }

    attackSize.x = 24;
    attackSize.y = 64;
    attackSize.z = 24;
    attackOffset.x = 0;
    attackOffset.y = 0;
    attackOffset.z = 0;
    for (i = 0; i < 3; i++) {
        void *c = &d->attack[i];
        Collider_Init(c, 0, 0x2401, 0, 16, &attackSize, &attackOffset);
        {
            u16 first = s->attacks.first[0];
            u16 second = s->attacks.second[0];
            u32 flags = s->attacks.flags[0];
            u16 parameter = s->attacks.parameter[0];
            Collider_SetAttackProperties(c, first, second, 0, flags, parameter);
        }
        Collider_SetPosition(c, &d->origin, 0);
        Collider_SetCallback(c, Axe_OnSecondaryColliderContact, d);
    }

    d->shadowActive = Shadow_Initialize(d->shadow, &d->draw.spatial.position, 0);
    ActorShadow_Disable(d->shadow);
}
