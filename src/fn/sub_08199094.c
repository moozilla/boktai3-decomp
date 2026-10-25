// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "display_animation.h"

struct EffectMessage {
    u32 id;
    u16 *data;
};
struct EffectRequest {
    u8 type, side, remaining, unk_03;
};
struct EffectActor {
    void *update, *destroy;
    u8 animation[0x20];
    u8 playback[0x12e];
    u8 finished, unk_157;
};
struct EffectRing {
    void *update, *destroy;
    u8 animation[0x1c];
    struct DisplaySequenceSprite sprite;
    struct DisplaySequenceState sequence;
    u8 unk_60[0x55];
    u8 finished;
    u8 unk_B6[2];
};
struct SceneEffectQueue {
    u8 unk_00[0x18];
    u32 messageId;
    u8 actorsUsed, ringsUsed, writeIndex, readIndex;
    u8 unk_20[0xc];
    struct EffectRequest requests[2];
    struct EffectActor actors[1];
    struct EffectRing rings[4];
};
s32 MessageQueue_GetMessagesById(u32, struct EffectMessage **);
void *SceneEffect_AllocateActor(struct SceneEffectQueue *, s32, s32);

static inline void QueueRequest(struct SceneEffectQueue *p, s32 type, s32 side, s32 count)
{
    s32 next = p->writeIndex;
    next++;
    if (next > 1)
        next -= 2;
    if (next != p->readIndex) {
        p->requests[p->writeIndex].type = type;
        p->requests[p->writeIndex].side = side;
        p->requests[p->writeIndex].remaining = count;
        p->writeIndex = next;
    }
}
static inline struct EffectRequest *PeekRequest(struct SceneEffectQueue *p)
{
    if (p->writeIndex == p->readIndex)
        return NULL;
    return &p->requests[p->readIndex];
}
static inline void PopRequest(struct SceneEffectQueue *p)
{
    if (p->writeIndex != p->readIndex) {
        s32 next = p->readIndex;
        next++;
        if (next > 1)
            next -= 2;
        p->readIndex = next;
    }
}
static inline u32 StepRing(struct EffectRing *e)
{
    struct DisplaySequenceSprite *o = &e->sprite;
    struct DisplaySequenceState *a = &e->sequence;
    const volatile u16 *entry = a->commands + a->index;
    u32 done;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        u32 left = 1;
        u32 mask = 1;
        left &= input;
        if (left != (((*entry & 0x30) >> 4) & mask))
            e->sprite.flags |= 4;
        else
            e->sprite.flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
        mask = 2;
    }
    if ((u16)++a->elapsed >= a->duration) {
        a->elapsed = 0;
        if (a->flags & 4) {
            if (a->index == 0) {
                if (a->flags & 8)
                    a->elapsed = a->duration;
                else
                    a->index = a->frameCount - 1;
                done = 1;
            } else {
                a->index--;
                done = 0;
            }
        } else {
            if (++a->index >= a->frameCount) {
                if ((u8)(a->flags & 8)) {
                    a->elapsed = a->duration;
                    a->index = a->frameCount - 1;
                } else
                    a->index = 0;
                done = 1;
            } else
                done = 0;
        }
        entry = a->commands + a->index;
        a->frameWeight = *entry & 15;
        a->duration = (a->frameWeight * a->speed) >> 6;
        if (!a->duration)
            a->duration = 1;
    } else
        done = 0;
    return done;
}


s32 SceneEffectQueue_UpdateEffects(struct SceneEffectQueue *p)
{
    struct EffectMessage *message;
    s32 n = MessageQueue_GetMessagesById(p->messageId, &message);
    struct EffectRequest *request;
    struct EffectActor *actor;
    struct EffectRing *ring;
    s32 i;
    u32 bit;
    for (; n > 0; n--, message++) {
        u16 *data = message->data;
        switch (data[0]) {
            case 0:
            case 1:
            case 2:
            case 3:
                QueueRequest(p, data[0], data[1], data[2]);
                break;
        }
    }
    request = PeekRequest(p);
    if (request && request->remaining) {
        if (SceneEffect_AllocateActor(p, request->type, request->side))
            request->remaining--;
    }
    actor = p->actors;
    for (bit = 1, i = 1; i > 0; i--, actor++, bit <<= 1) {
        if (actor->update)
            Callback_InvokeActor(actor, actor->update);
        if (!*(void *volatile *)&actor->update) {
            p->actorsUsed &= ~bit;
            if (!PeekRequest(p)->remaining)
                PopRequest(p);
        } else
            actor->finished = Anim_Step(actor->playback, actor->animation);
    }
    ring = p->rings;
    for (bit = 1, i = 4; i > 0; i--, ring++, bit <<= 1) {
        if (ring->update)
            Callback_InvokeActor(ring, ring->update);
        if (!*(void *volatile *)&ring->update)
            p->ringsUsed &= ~bit;
        else
            ring->finished = StepRing(ring);
    }
    return 0;
}
