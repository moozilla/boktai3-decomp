#include "scene_actor_chase_reaction.h"
#include "scene_actor_contact_list.h"
#include "scene_actor_reaction.h"

void SceneActorReaction_ResetResponseState(void *, const void *, u32);
void SceneActorTarget_ResetContactResponse(void *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
extern const u8 gUnk_0824E230[];

struct VornContactTarget {
    struct SceneActorChaseTarget target;
    u8 unknown14[8];
    struct SceneActorChasePlayerCell *cell;
};

struct VornContactTracking {
    struct SceneActorChaseTracking tracking;
    u32 active;
    u32 *actionFlags;
};

static inline u32 Vorn_ReadActorFlags(struct SceneActorRegistryResetView *actor, u32 mask,
                                      u32 **field)
{
    u32 *flags = &actor->flags2CC;
    u32 value = *flags & mask;
    *field = flags;
    return value;
}
static inline u32 Vorn_MotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
static inline u8 Vorn_TestTracking(struct VornContactTracking *motion, u32 mask)
{
    if (motion->tracking.flags & mask)
        return 1;
    return 0;
}
static inline void Vorn_SetTrackingFlags(struct VornContactTracking *motion, u8 bits)
{
    motion->tracking.flags = bits | motion->tracking.flags;
}
static inline u16 Vorn_SavedTargetCell(struct SceneActorRegistryResetView *actor)
{
    return *(u16 *)actor->unknown2F8;
}
static inline void Vorn_SaveTargetCell(struct SceneActorRegistryResetView *actor, u16 cell)
{
    *(u16 *)actor->unknown2F8 = cell;
}

static inline u8 Vorn_TestTrackingOwned(struct VornContactTracking *current, u32 mask,
                                        struct VornContactTracking **motion)
{
    u32 value = current->tracking.flags & mask;
    *motion = current;
    if (value)
        return 1;
    return 0;
}
static inline u32 Vorn_QueryMotionField(struct SceneActorRegistryResetView *actor, u16 mask,
                                        void **owner)
{
    u16 *field = &actor->flags2D4;
    u32 value;
    *owner = field;
    value = *field & mask;
    return value;
}

static inline u16 *Vorn_MotionField(struct SceneActorRegistryResetView *actor)
{
    return &actor->flags2D4;
}
static inline struct SceneActorChaseFlags16 *Vorn_GateFromWork(void *const *work)
{
    return &((struct SceneActorRegistryResetView *)*work)->group.common.gate.flags;
}
static inline struct SceneActorRegistryResetView *Vorn_ActorFromWork(void *const *work)
{
    return *work;
}
#define VORN_READ_MOTION_FIELD(out_, field_)                                                       \
    do {                                                                                           \
        (out_) = *(field_);                                                                        \
    } while (0)
#define VORN_FIND_CONTACT(out_, actor_)                                                            \
    do {                                                                                           \
        (out_) = SceneActorContacts_SelectForActor((void *)(actor_), 6);                           \
    } while (0)
u8 Vorn_UpdateContactResponse(struct SceneActorRegistryResetView *actor)
{
    struct VornContactTracking *motion;
    u16 zero;
    u16 *motionFlags;
    void *work;
    void **nodeSlot;
    struct SceneActorContactNode *contact;
    struct VornContactTarget *target;
    u16 cell;
    u32 active;
    u32 *actionFlags;
    work = actor;
    if (Vorn_MotionFlags(Vorn_ActorFromWork(&work), 0x10))
        return 0;
    if ((u8)Callback_InvokeActor(actor, actor->update26C))
        return 1;
    if (!Vorn_TestTrackingOwned((void *)&actor->tracking, 1, &motion))
        return 0;
    motionFlags = Vorn_MotionField(actor);
    {
        u16 flags;
        VORN_READ_MOTION_FIELD(flags, motionFlags);
        zero = flags & 0x100;
    }
    if (zero)
        return 0;
    nodeSlot = &actor->group.contactNode;
    if (!actor->tracking.limit)
        return 0;
    VORN_FIND_CONTACT(contact, actor);
    if (!contact)
        return 0;
    if (Vorn_QueryMotionField(actor, 0x1000, &work)) {
        SceneActorContacts_ReleaseNode(*nodeSlot);
        SceneActorChase_ClearBits(work, 0x1000);
        *nodeSlot = (void *)zero;
    }
    target = contact->object;
    motion->tracking.target = &target->target;
    cell = target->cell->index;
    work = actor;
    SceneActorChase_ClearFlagComponent(Vorn_GateFromWork(&work), 2);
    if (motion->active) {
        if (Vorn_TestTracking(motion, 8)) {
            if (Vorn_SavedTargetCell(actor) != cell)
                Vorn_SaveTargetCell(actor, cell);
        } else
            SceneActorTarget_ResetContactResponse(actor);
        SceneActorChase_SetReactionFlags(actor, 0x80);
        Vorn_SetTrackingFlags(motion, 0x10);
        return 0;
    }
    active = Vorn_ReadActorFlags(actor, 0x80, &actionFlags);
    if (active)
        return 0;
    *actionFlags |= 0x80;
    SceneActorChase_ClearBits(motionFlags, 0x100);
    SceneActorReaction_ResetResponseState(&actor->group.saved, gUnk_0824E230, 0x689);
    SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
    SceneActorChase_SetState(actor, 3, 1, 0);
    SceneActorChase_ClearBits(motionFlags, 0x400);
    actor->group.turn.turnsRemaining = -1;
    SceneActorAnimation_SelectDirectionalEntry(Vorn_ActorFromWork(&work), 0);
    actor->group.common.control.timer = 0xB4;
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, actor->category);
    SceneActorChase_SetReactionFlags(actor, 0x100);
    return 1;
}
