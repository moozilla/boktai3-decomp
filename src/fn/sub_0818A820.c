#include "global.h"
#include "engine.h"
#include "scene_actor_components.h"
#include "scene_actor_registry.h"
#include "scene_actor_terrain.h"
#include "scene_body_api.h"
#include "actor_attachment_manager.h"

struct ArmorCallbackSlot {
    void *callback;
    u8 unknown4[0x3C];
};
struct ArmorCollisionControl {
    u32 unknown0, flags;
};
struct ArmorUpdateActor {
    u8 unknown0[0x3C];
    struct SceneActorBody body;
    u8 probeFlags;
    u8 unknown85[0x51];
    u16 groundHeight;
    u8 unknownD8[0xA8];
    struct ArmorCollisionControl collisionControl;
    u8 unknown188[8];
    struct ContactCollider colliderB, colliderA, colliderC, colliderD;
    u8 unknown2E0[0x70];
    struct ArmorCallbackSlot callback350, callback390, callback3D0;
    struct ArmorCallbackSlot slots[9];
    struct ArmorCallbackSlot callback650;
    u8 unknown690[0x480];
    s16 recoilX;
    u8 unknownB12[2];
    s16 recoilZ;
    u8 unknownB16[2];
    u32 messageId;
    u8 unknownB1C[0x10];
    s16 timer;
    u8 unknownB2E[4];
    u16 flags;
    u8 unknownB34[5];
    u8 facing;
    u8 unknownB3A[2];
    u8 behavior, phase;
    u8 unknownB3E[8];
    u16 attachmentTimer;
    struct ActorAttachmentSlotView *attachment;
    u8 unknownB4C[0xC];
    u8 effectPool[0x5C4];
};
s32 MessageQueue_GetMessagesById(u32, struct SceneActorRegistryMessage **);
void ActorController_SetBehavior(void *, s32);
void Armor_ApplySceneCommands(void *);
void Armor_DispatchTypeUpdate(void *);
void Armor_DispatchBehaviorUpdate(void *);
void ActorController_UpdateAnimation(void *);
void SceneBody_ClearMovementProbe(struct SceneActorBody *);
u32 sub_0821C254(void *, const struct ContactPosition *, struct ContactPosition *, u32);
void Obj_StepPosition(struct SceneActorBody *);
void Armor_StartAccentPaletteFade(void *);
void SceneEffectPool_Update(void *, const struct ContactPosition *);

static inline s32 Armor_GroundHeight(const struct ContactPosition *position)
{
    u8 *data = SceneActorTerrain_GetCell(SceneActorTerrain_GetCellIndex(position));
    u32 kind = *data >> 4;
    s32 height = (*data & 15) << 8;
    switch (kind) {
        case 1:
            height -= ((const u8 *)position)[4];
            break;
        case 2:
            height -= ((const u8 *)position)[0];
            break;
    }
    return height;
}

static inline void Armor_Decay(s16 *value)
{
    s16 v = *value;
    if (v == -1)
        *value = 0;
    else
        *value = v >> 1;
}

static inline void Armor_ClearCollisionControl(struct ArmorCollisionControl *control)
{
    u32 mask = ~1;
    control->flags &= mask;
}
static inline void Armor_SetCollisionControl(struct ArmorCollisionControl *control)
{
    u32 mask = 1;
    control->flags |= mask;
}
static inline void *Armor_MovementProbe(struct SceneActorBody *body)
{
    return body->movementProbe;
}
static inline void Armor_SubtractPosition(struct ContactPosition *out,
                                          const struct ContactPosition *a,
                                          const struct ContactPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
static inline void Armor_InvokeSlot(struct ArmorCallbackSlot *slot)
{
    if (slot->callback != NULL)
        Callback_InvokeActor(slot, slot->callback);
}

static inline void Armor_ResolveMotion(void *probe, struct ContactPosition *position,
                                       struct ContactPosition *motion,
                                       struct ArmorUpdateActor *actor)
{
    union SceneActorRegistryPackedPosition candidate;
    candidate.packed.x = position->x + motion->x;
    candidate.packed.y = position->y + motion->y;
    candidate.packed.z = position->z + motion->z;
    candidate.packed.padding = 16;
    if (sub_0821C254(probe, position, &candidate.position, actor->body.mode)) {
        Armor_SubtractPosition(motion, &candidate.position, position);
    }
}

/* Drain messages, resolve body motion, and advance Armor components. */
s32 sub_0818A820(void *arg0)
{
    struct ArmorUpdateActor *actor = arg0;
    struct SceneActorRegistryMessage *message;
    s32 count;
    s32 i;

    count = MessageQueue_GetMessagesById(actor->messageId, &message);
    while (count > 0) {
        switch (*(u16 *)message->data) {
            case 0:
                if (actor->flags & 0x8000) {
                    actor->flags &= 0x73FF;
                    if (actor->behavior == 0x2C) {
                        ActorController_SetBehavior(actor, 0x2D);
                    } else {
                        ActorController_SetBehavior(actor, 1);
                        actor->phase = 0xFF;
                    }
                    actor->colliderA.flags &= 0xFFFB;
                    actor->colliderC.flags &= 0xFFFB;
                    Armor_ClearCollisionControl(&actor->collisionControl);
                }
                break;
            case 1:
                if (!(actor->flags & 0x8000)) {
                    actor->flags |= 0x8000;
                    ActorController_SetBehavior(actor, 0);
                    actor->colliderA.flags |= 4;
                    actor->colliderC.flags |= 4;
                    Armor_SetCollisionControl(&actor->collisionControl);
                }
                break;
            case 2:
                if (actor->flags & 0x8000) {
                    actor->flags &= 0xF3FF;
                    if (actor->behavior == 0x2C)
                        ActorController_SetBehavior(actor, 0x2D);
                }
                break;
            case 3:
                if (!(actor->flags & 0x1000)) {
                    actor->flags |= 0x1000;
                    actor->colliderA.flags |= 4;
                    actor->colliderC.flags |= 4;
                    Armor_SetCollisionControl(&actor->collisionControl);
                }
                break;
            case 4:
                if (actor->flags & 0x1000)
                    actor->flags &= 0xEFFF;
                break;
        }
        count--;
        message++;
    }

    if (actor->flags & 0x1000)
        return 0;

    if (actor->timer > 0)
        actor->timer--;
    if (actor->flags & 0x8000)
        Armor_ApplySceneCommands(actor);
    else
        Armor_DispatchTypeUpdate(actor);
    Armor_DispatchBehaviorUpdate(actor);
    ActorController_UpdateAnimation(actor);

    if (actor->flags & 0x40) {
        actor->body.motion.x += actor->recoilX;
        actor->body.motion.z += actor->recoilZ;
        Armor_Decay(&actor->recoilX);
        Armor_Decay(&actor->recoilZ);
        if (actor->recoilX == 0 && actor->recoilZ == 0)
            actor->flags &= 0xFFBF;
    }

    if (actor->flags & 0x60) {
        if (Armor_MovementProbe(&actor->body) == NULL)
            SceneBody_SetMovementProbe(&actor->body, &actor->probeFlags, 0x40, 0x40);
    } else if (Armor_MovementProbe(&actor->body) != NULL) {
        SceneBody_ClearMovementProbe(&actor->body);
    }

    if (Armor_MovementProbe(&actor->body) != NULL) {
        Armor_ResolveMotion(actor->body.movementProbe, &actor->body.position, &actor->body.motion,
                            actor);
    }
    Obj_StepPosition(&actor->body);

    if (Armor_MovementProbe(&actor->body) != NULL) {
        if (actor->probeFlags & 1)
            actor->flags |= 0x10;
        else
            actor->flags &= 0xFFEF;
    } else {
        actor->flags &= 0xFFEF;
    }

    if (Armor_MovementProbe(&actor->body) == NULL) {
        s32 height = Armor_GroundHeight(&actor->body.position);
        actor->body.position.y = height;
        actor->groundHeight = height;
    }

    Collider_Refresh(&actor->colliderA);
    Collider_Refresh(&actor->colliderB);
    Collider_SetPosition(&actor->colliderA, &actor->body.position, 0);
    Collider_SetPosition(&actor->colliderC, &actor->body.position, 0);
    Collider_SetPosition(&actor->colliderD, &actor->body.position, 0);
    Collider_SetPosition(&actor->colliderB, &actor->body.position, 0);
    actor->colliderA.direction = actor->facing;
    actor->colliderC.direction = actor->facing;

    if (actor->callback350.callback != NULL)
        Callback_InvokeActor(&actor->callback350, actor->callback350.callback);
    if (actor->callback390.callback != NULL)
        Callback_InvokeActor(&actor->callback390, actor->callback390.callback);
    if (actor->callback3D0.callback != NULL)
        Callback_InvokeActor(&actor->callback3D0, actor->callback3D0.callback);
    for (i = 0; i <= 8; i++) {
        Armor_InvokeSlot(&actor->slots[i]);
    }
    Armor_StartAccentPaletteFade(actor);
    if (actor->callback650.callback != NULL)
        Callback_InvokeActor(&actor->callback650, actor->callback650.callback);
    SceneEffectPool_Update(actor->effectPool, &actor->body.position);

    if (actor->attachment != NULL) {
        actor->attachmentTimer--;
        if ((s16)actor->attachmentTimer <= 0) {
            ActorAttachment_ReleaseResource(actor->attachment, 1, (void *)actor->messageId);
            ActorAttachment_ReleaseResource(actor->attachment, 0, (void *)actor->messageId);
            actor->attachment = NULL;
        }
    }
    return 0;
}
