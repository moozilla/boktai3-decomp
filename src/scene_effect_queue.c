#include "scene_effect_queue.h"
#include "global.h"
#include "display_animation.h"
#include "display_sprite_playback.h"
#include "ram.h"
#include "game_state.h"
#include "scene_actor_registry.h"
#include "scene_effect_projection.h"

s32 MessageQueue_GetMessagesById(u32, struct SceneEffectMessage **);

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

static inline struct SceneEffectRequest *PeekRequest(struct SceneEffectQueue *p)
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

static inline void RunSlotCallback(void *slot, void *callback)
{
    if (callback)
        Callback_InvokeActor(slot, callback);
}

s32 SceneEffectQueue_UpdateEffects(struct SceneEffectQueue *p)
{
    struct SceneEffectMessage *message;
    s32 n = MessageQueue_GetMessagesById(p->messageId, &message);
    struct SceneEffectRequest *request;
    struct SceneEffectActor *actor;
    struct SceneEffectRing *ring;
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
        RunSlotCallback(actor, actor->update);
        if (!actor->update) {
            p->actorsUsed &= ~bit;
            if (!PeekRequest(p)->remaining)
                PopRequest(p);
        } else
            actor->animationEnded = Anim_Step(&actor->animation, actor->animationData);
    }
    ring = p->rings;
    for (bit = 1, i = 4; i > 0; i--, ring++, bit <<= 1) {
        RunSlotCallback(ring, ring->update);
        if (!ring->update)
            p->ringsUsed &= ~bit;
        else
            ring->animationEnded = PlayerCue_StepSequenceEnded(&ring->sprite, &ring->sequence);
    }
    return 0;
}

s32 CallbackPool_Destroy(struct SceneEffectQueue *pool)
{
    s32 i;
    for (i = 0; i <= 0; i++) {
        u32 address = i;
        struct SceneEffectActor *entry;
        address *= sizeof(struct SceneEffectActor);
        address += (u32)pool;
        address += (u32) & ((struct SceneEffectQueue *)0)->actors;
        entry = (struct SceneEffectActor *)address;
        entry->update = 0;
        if (entry->destroy) {
            Callback_InvokeActor(entry, entry->destroy);
            entry->destroy = 0;
        }
    }
    for (i = 0; i <= 3; i++) {
        u32 address = i;
        struct SceneEffectRing *slot;
        address *= sizeof(struct SceneEffectRing);
        address += (u32)pool;
        address += (u32) & ((struct SceneEffectQueue *)0)->rings;
        slot = (struct SceneEffectRing *)address;
        slot->update = 0;
        if (slot->destroy) {
            Callback_InvokeActor(slot, slot->destroy);
            slot->destroy = 0;
        }
    }
    return (u32)(gSceneEffectQueue = 0);
}

s32 SceneEffect_InitializeEmitterOptions(struct SceneEffectQueue *emitter)
{
    struct SceneEffectActor *actor;
    struct SceneEffectRing *ring;
    s32 i;
    u32 zero;
    u32 stride;
    emitter->actorsUsed = 0;
    emitter->ringsUsed = 0;
    emitter->writeIndex = 0;
    emitter->readIndex = 0;
    emitter->actorPower = 3;
    emitter->actorElement = 1;
    emitter->ringPower = 5;
    emitter->ringElement = 3;
    emitter->specialPower = 10;
    emitter->specialElement = 10;
    if (Script_SeekToKeyword('b')) {
        emitter->actorPower = Script_GetValue();
        emitter->actorElement = Script_GetValue();
    }
    if (Script_SeekToKeyword('r')) {
        emitter->ringPower = Script_GetValue();
        emitter->ringElement = Script_GetValue();
    }
    if (Script_SeekToKeyword('s')) {
        emitter->specialPower = Script_GetValue();
        emitter->specialElement = Script_GetValue();
    }
    zero = 0;
    actor = emitter->actors;
    stride = sizeof(*actor);
    i = 0;
    do {
        actor->update = zero;
        actor->destroy = zero;
        actor = (struct SceneEffectActor *)((u8 *)actor + stride);
    } while (--i >= 0);
    zero = 0;
    ring = emitter->rings;
    i = 3;
    do {
        ring->update = zero;
        ring->destroy = zero;
        ring++;
    } while (--i >= 0);
    return 0;
}

struct SceneEffectQueue *SceneEffectQueue_Create(u16 a)
{
    struct SceneEffectQueue *p;
    if (gSceneEffectQueue != 0)
        return 0;
    p = Task_Create(8, sizeof(*p));
    if (p != 0) {
        p->messageId = a;
        Task_SetCallbacks(p, SceneEffectQueue_UpdateEffects, CallbackPool_Destroy);
        if (SceneEffect_InitializeEmitterOptions(p) < 0) {
            Task_Kill(p);
            return 0;
        }
        gSceneEffectQueue = (u8 *)p;
    }
    return p;
}

void MessageQueue_PostMessage(struct SceneEffectPostedMessage *);

static inline void PostSceneEffect(s32 value, u32 type)
{
    s16 values[3];
    struct SceneEffectPostedMessage message;
    union SceneActorRegistryPackedPosition in;

    message.id = 0x382f;
    message.count = 3;
    message.values = values;
    in.packed.x = (u16)gGameState->positionX;
    in.packed.y = (u16)gGameState->positionY;
    in.packed.z = (u16)gGameState->positionZ;
    SET_PROJECTED_EFFECT_SIDE(&in.position, values[1]);

    values[0] = type;
    values[2] = value;
    MessageQueue_PostMessage(&message);
}

void SceneEffectQueue_PostTypeZeroMessage(s32 value)
{
    PostSceneEffect(value, 0);
}

void SceneEffectQueue_PostTypeOneMessage(s32 value)
{
    PostSceneEffect(value, 1);
}

void SceneEffectQueue_PostTypeThreeMessage(s32 value)
{
    PostSceneEffect(value, 3);
}

s32 SceneEffectQueue_QueueTypeZeroEffect(void)
{
    union SceneActorRegistryPackedPosition position;
    struct SceneEffectQueue *queue;
    s32 next, side;
    if (!gSceneEffectQueue || (gEnginePauseFlags & 7))
        goto fail;
    position.packed.x = (u16)gGameState->positionX;
    position.packed.y = (u16)gGameState->positionY;
    position.packed.z = (u16)gGameState->positionZ;
    SET_PROJECTED_EFFECT_SIDE(&position.position, side);
    queue = (struct SceneEffectQueue *)gSceneEffectQueue;
    next = queue->writeIndex;
    next++;
    if (next > 1)
        next -= 2;
    if (next != queue->readIndex)
        goto write;
fail:
    return 0;
write: {
    struct SceneEffectQueue *base = queue;
    struct SceneEffectQueue **owner = &base;
    (*owner)->requests[queue->writeIndex].type = 0;
    (*owner)->requests[queue->writeIndex].side = side;
    (*owner)->requests[queue->writeIndex].remaining = 1;
    queue->writeIndex = next;
    return 1;
}
}

s32 SceneEffectQueue_QueueTypeOneEffect(void)
{
    union SceneActorRegistryPackedPosition position;
    struct SceneEffectQueue *queue;
    s32 next, side;
    u32 one;
    if (!gSceneEffectQueue || (gEnginePauseFlags & 7))
        goto fail;
    position.packed.x = (u16)gGameState->positionX;
    position.packed.y = (u16)gGameState->positionY;
    position.packed.z = (u16)gGameState->positionZ;
    SET_PROJECTED_EFFECT_SIDE(&position.position, side);
    queue = (struct SceneEffectQueue *)gSceneEffectQueue;
    one = 1;
    next = queue->writeIndex;
    next++;
    if (next > 1)
        next -= 2;
    if (next != queue->readIndex)
        goto write;
fail:
    return 0;
write: {
    struct SceneEffectQueue *base = queue;
    struct SceneEffectQueue **owner = &base;
    (*owner)->requests[queue->writeIndex].type = one;
    (*owner)->requests[queue->writeIndex].side = side;
    (*owner)->requests[queue->writeIndex].remaining = one;
    queue->writeIndex = next;
    return 1;
}
}

s32 SceneEffectQueue_QueueTypeTwoEffect(void)
{
    union SceneActorRegistryPackedPosition position;
    struct SceneEffectQueue *queue;
    s32 next, side;
    if (!gSceneEffectQueue || (gEnginePauseFlags & 7))
        goto fail;
    position.packed.x = (u16)gGameState->positionX;
    position.packed.y = (u16)gGameState->positionY;
    position.packed.z = (u16)gGameState->positionZ;
    SET_PROJECTED_EFFECT_SIDE(&position.position, side);
    queue = (struct SceneEffectQueue *)gSceneEffectQueue;
    next = queue->writeIndex;
    next++;
    if (next > 1)
        next -= 2;
    if (next != queue->readIndex)
        goto write;
fail:
    return 0;
write: {
    struct SceneEffectQueue *base = queue;
    struct SceneEffectQueue **owner = &base;
    (*owner)->requests[queue->writeIndex].type = 2;
    (*owner)->requests[queue->writeIndex].side = side;
    (*owner)->requests[queue->writeIndex].remaining = 1;
    queue->writeIndex = next;
    return 1;
}
}
