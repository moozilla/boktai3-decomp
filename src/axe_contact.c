#include "global.h"
#include "axe.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_behavior.h"
#include "scene_actor_placement.h"
#include "scene_actor_chase_shared.h"
#include "scene_actor_reaction.h"
#include "engine.h"
#include "ram.h"
#include "player.h"
#include "contact_swarm.h"
#include "movement_curve.h"

void Axe_OnSecondaryColliderContact(void)
{
}

struct AxeContactState {
    u8 pad0[0x134];
    struct {
        u16 v[4];
        u8 pad[0x54 - 8];
    } body;
    struct {
        u16 v[4];
    } attack;
    u8 pad190[0xcab - 0x190];
    u8 flags;
};

// confidence=low: contact sets a latch and disables both actor colliders.

void Axe_LatchColliderContact(void *a, void *b, struct AxeContactState *s)
{
    if (!(s->flags & 1)) {
        s->flags |= 1;
        {
            typeof(s->attack) *p = &s->attack;
            p->v[3] |= 4;
        }
        {
            typeof(s->body) *p = &s->body;
            p->v[3] |= 4;
        }
    }
}

void Axe_OnAttackColliderContact(void)
{
}

void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *, void *);

void ActorPhase_UpdateReverseFlicker(void);

static inline u16 Axe_ContactResponse(const struct SceneActorBehaviorContactView *contact)
{
    return contact->parameter44;
}

static inline s32 Axe_ReadContactIndex(const s32 *index)
{
    return *index;
}

static inline s32 Axe_ContactIndex(s32 index)
{
    return Axe_ReadContactIndex(&index);
}

// Records the first contact response, then disarms the active collider banks

// and enters reverse flicker when the armed contact timer exceeds two.

void Axe_OnBodyContact(struct SceneActorBehaviorContactView *source,
                       struct SceneActorBehaviorContactView *destination, void *object)
{
    struct ChaseActor *actor = object;
    struct Axe *context = actor->base.context;
    if (SceneActorPlacement_IsUnavailable((void *)actor))
        return;
    SceneActorBehavior_ApplyContactReaction(source, destination, actor);
    if (actor->base.colliderA.responseValue == 0) {
        actor->base.colliderA.cost = destination->remaining;
        actor->base.field2E6 = destination->mode;
        actor->base.colliderA.responseValue = Axe_ContactResponse(destination);
    }
    if ((context->flagsCAB & 2) && context->reactionCount > 2) {
        s32 i;
        u32 stride = sizeof(struct ContactCollider);
        i = 0;
        do {
            u32 attackStride = sizeof(context->attack[0]);
            struct ContactCollider *attackBase = context->attack;
            struct ContactCollider *attack = (void *)((u8 *)attackBase + i * attackStride);
            /* Capture the body bank record layout at this iteration. */
            typedef u8 AxeBodyRecord[stride];
            struct ContactCollider *body =
                (struct ContactCollider *)&((AxeBodyRecord *)context->body)[i];
            attack->flags |= 4;
            body->flags |= 4;
            ++i;
        } while (Axe_ContactIndex(i) <= 2);
        SceneActorChase_SetControl(actor, (void *)ActorPhase_UpdateReverseFlicker, 0x2F);
        context->reactionCount = 0;
    }
}

#define SceneActor_ConfigureTrailSegments Axe_SetAuxiliarySpriteRenderState

struct SceneActorApproachContext {
    u8 unknown0[0x888], trail[0x19C];
    struct ContactPosition current;
    u8 unknownA2C[8], motionMode, unknownA35[0x253];
    struct ContactPosition destination;
    u8 unknownC90[0x1B], flagsCAB, unknownCAC[0x195], duration;
    u8 unknownE42[0x6D], remaining, unknownEB0[0x22], stateED2;
};

void Axe_SetAuxiliarySpriteRenderState(struct ChaseActor *, void *, u32);

void ActorPhase_EmitTrailSprite(struct ChaseActor *, struct ContactPosition *, u32);

void Axe_UpdateTrailColliders(struct ChaseActor *);

struct PhaseActor;

void ActorPhase_UpdateTimedFlicker(struct PhaseActor *s, s32 frame);

s32 Div(s32, s32);

static inline void SceneActor_SubtractApproachPosition(struct ContactPosition *out,
                                                       struct ContactPosition *a,
                                                       struct ContactPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

static inline void SceneActor_SetApproachControl(struct ChaseActor *p, void *control, u32 kind)
{
    p->base.field2B2 = 1;
    p->base.stepDone = 0;
    p->base.controlKind = kind;
    p->base.target = control;
    p->base.field2C8 = 0;
    {
        u32 *flags = &p->base.pendingControlFlags;
        u32 mask = ~1;
        *flags &= mask;
    }
    p->base.pendingControl = 0;
}

void SceneActor_AdvanceOffsetApproach(struct ChaseActor *p)
{
    struct SceneActorApproachContext *context = p->base.context;
    struct ContactPosition difference;
    struct ContactPosition *current, *destination;
    if (SceneActorReaction_TakePrepared(p)) {
        context->stateED2 = 0;
        context->destination = p->base.body.position;
        context->destination.x -= 256;
        context->current = p->base.body.position;
        context->motionMode = 0;
        context->remaining = context->duration;
        context->flagsCAB = 0;
        Axe_SetAuxiliarySpriteRenderState(p, context->trail, 316);
    }
    current = &context->current;
    ActorPhase_EmitTrailSprite(p, current, context->motionMode);
    Axe_UpdateTrailColliders(p);
    SceneActor_SubtractApproachPosition(&difference, &context->destination, current);
    current->x += Div(difference.x, context->remaining);
    context->remaining--;
    if (!context->remaining) {
        SceneActor_SetApproachControl(p, ActorPhase_UpdateTimedFlicker, 43);
    }
}

#define ActorPhase_SetAnimation Axe_SetAuxiliarySpriteParameters

struct PhaseActor {
    u8 pad0[0x104];
    u32 f104;
    u8 pad108[0xc];
    u32 flags114;
    u8 pad118[0x104];
    void (*phase)(void);
    u8 pad220[0x90];
    u8 phaseState;
    u8 pad2b1;
    u8 entered;
    u8 pad2b3[2];
    u8 phaseId;
    u8 pad2b6[0x12];
    u16 timer;
    u8 pad2ca[0x106];
    u8 *data;
};

void Axe_InitAttackTrail(struct PhaseActor *);

void Axe_CopyAuxiliaryVector(struct PhaseActor *, void *);

void ActorShadow_Enable(void *);

void Sound_PlaySong(u32);

u8 ActorPhase_UpdateFlicker(u32 *, s32, s32, s32);

void Axe_SetAuxiliarySpriteParameters(struct PhaseActor *, void *, u32);

void Axe_UpdateOrbitAttack(struct ChaseActor *p, s32 frame);

static inline u8 consumeEntry(struct PhaseActor *s)
{
    if (s->entered) {
        s->entered = 0;
        return 1;
    }
    return 0;
}

static inline void clearFlags(u32 *p, u32 mask)
{
    *p &= ~mask;
}

void ActorPhase_UpdateTimedFlicker(struct PhaseActor *s, s32 frame)
{
    u8 *data = s->data;
    u32 flags;
    if (consumeEntry(s)) {
        Axe_InitAttackTrail(s);
        Axe_CopyAuxiliaryVector(s, data + 0xc88);
        *(u16 *)(data + 0x90a) = 0;
        *(u32 *)(data + 0xa38) &= ~1;
        ActorShadow_Enable(data + 0xcac);
        Axe_SetAuxiliarySpriteParameters(s, data + 0x888, 0x13c);
        Sound_PlaySong(0x16f);
    }
    if (ActorPhase_UpdateFlicker(&flags, frame, 10, 30)) {
        void (*next)(void) = Axe_UpdateOrbitAttack;
        u32 id = 0x2c;
        s->entered = 1;
        s->phaseState = 0;
        s->phaseId = id;
        s->phase = next;
        s->timer = 0;
        clearFlags(&s->flags114, 1);
        s->f104 = 0;
        Axe_SetAuxiliarySpriteParameters(s, data + 0x908, 0x13f);
        Sound_PlaySong(0x16d);
    } else if (flags & 1) {
        Axe_SetAuxiliarySpriteParameters(s, data + 0x888, 0x13c);
    } else {
        Axe_SetAuxiliarySpriteParameters(s, data + 0x908, 0x13f);
    }
}

extern const s16 gUnk_086149C4[];

s32 Mod(s32, s32);

s32 Math_Atan2(s32, s32);

void Axe_UpdateAttackColliderPositions(void *);

void ActorPhase_UpdateAcceleratingTrail(void);

static inline void Axe_AddPositions(struct ContactPosition *out, struct ContactPosition *a,
                                    struct ContactPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}

static inline s32 Axe_ScaleTowardZero(s32 value, s32 bits)
{
    if (value >= 0)
        return value >> bits;
    return -(-value >> bits);
}

static inline s32 Axe_RandomByte(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u8 *)&((const u16 *)RANDOM_TABLE_ADDR)[index];
}

/* Orbit the actor and its auxiliary sprite around a moving shared center. */

void Axe_UpdateOrbitAttack(struct ChaseActor *p, s32 frame)
{
    struct Axe *data = p->base.context;
    struct ContactPosition delta;
    struct ContactPosition *position;
    u8 entered;
    s32 difference, absolute;
    entered = SceneActorControl_TakeFlag(&p->base.field2B2);
    position = &p->base.body.position;
    if (entered) {
        data->flagsCAB |= 2;
        data->angle = 0;
        data->radius = 128;
        data->speed = 4;
        data->direction = 1;
        data->center = data->origin;
        MovementCurve_SubtractPositions(&delta, &data->center, position);
        delta.x = Axe_ScaleTowardZero(delta.x, 1);
        delta.z = Axe_ScaleTowardZero(delta.z, 1);
        Axe_AddPositions(&data->center, &data->center, &delta);
    }
    position->x = data->center.x +
                  Axe_ScaleTowardZero(gUnk_086149C4[(data->angle + 64) & 255] * data->radius, 12);
    p->base.body.position.z =
        data->center.z + Axe_ScaleTowardZero(gUnk_086149C4[data->angle] * data->radius, 12);
    data->auxiliarySprite.position.x =
        data->center.x +
        Axe_ScaleTowardZero(gUnk_086149C4[(((data->angle + 128) & 255) + 64) & 255] * data->radius,
                            12);
    data->auxiliarySprite.position.z =
        data->center.z +
        Axe_ScaleTowardZero(gUnk_086149C4[(data->angle + 128) & 255] * data->radius, 12);
    if (frame < 60) {
        data->speed += 2;
        data->reactionCount = 0;
    } else if (frame < 180) {
        if (Mod(frame, 4) == 0) {
            if (Axe_RandomByte() <= 19) {
                {
                    s32 direction = data->direction;
                    data->direction = -direction;
                }
                Sound_PlaySong(0x169);
            }
        }
        MovementCurve_SubtractPositions(&delta, &data->center, (void *)&gPlayers[0]->position);
        data->heading = Math_Atan2(delta.x, delta.z);
        data->reactionCount = 0;
    } else {
        if (!(data->flagsCAB & 4)) {
            if (data->radius < 512) {
                data->radius += 8;
                data->speed -= 4;
                if (data->speed <= 15)
                    data->speed = 16;
            } else {
                data->radius = 512;
                data->speed = 16;
                data->flagsCAB |= 4;
            }
        }
        MovementCurve_SubtractPositions(&delta, &data->center, (void *)&gPlayers[0]->position);
        difference = Math_Atan2(delta.x, delta.z) - data->heading;
        if (difference > 128)
            difference -= 256;
        else if (difference < -128)
            difference += 256;
        absolute = difference;
        if (difference < 0)
            absolute = -difference;
        if (absolute > 20) {
            if (difference > 0)
                data->heading += 4;
            else
                data->heading -= 4;
        }
        data->center.x += Axe_ScaleTowardZero(gUnk_086149C4[(data->heading + 64) & 255] * 2, 12);
        data->center.z += Axe_ScaleTowardZero(gUnk_086149C4[data->heading] * 2, 12);
        if (data->flagsCAB & 4) {
            struct DisplaySequenceSprite *sprite = p->base.drawable;
            u32 previous;
            sprite->flags |= 2;
            data->auxiliarySprite.flags |= 2;
            previous = sprite->attribute;
            sprite->attribute = previous - 6;
            data->auxiliarySprite.attribute += 6;
            if ((s32)previous < ((struct DisplaySequenceSprite *)p->base.drawable)->attribute)
                Sound_PlaySong(0x169);
            if (data->flagsCAB & 4) {
                if (frame < 480)
                    data->radius--;
                else if (frame < 580)
                    data->radius += 6;
                else if (frame < 660)
                    data->radius -= 4;
                else if (frame < 740)
                    data->radius += 4;
                else if (frame > 819)
                    data->radius -= 6;
            }
        }
    }
    data->angle += Axe_ScaleTowardZero(data->speed * (s8)data->direction, 4);
    Axe_UpdateAttackColliderPositions(p);
    if (data->flagsCAB & 1) {
        SceneActorChase_SetControl(p, ActorPhase_UpdateAcceleratingTrail, 0x2E);
    } else if (frame > 1000) {
        s32 i;
        u32 stride = sizeof(struct ContactCollider);
        {
            struct ContactCollider *c = &p->base.colliderB;
            c->flags |= 4;
        }
        {
            struct ContactCollider *c = &p->base.colliderA;
            c->flags |= 4;
        }
        i = 0;
        do {
            u32 attackStride = sizeof(data->attack[0]);
            struct ContactCollider *attackBase = data->attack;
            struct ContactCollider *attack = (void *)((u8 *)attackBase + i * attackStride);
            typedef u8 AxeBodyRecord[stride];
            struct ContactCollider *body =
                (struct ContactCollider *)&((AxeBodyRecord *)data->body)[i];
            attack->flags |= 4;
            body->flags |= 4;
            ++i;
        } while (Axe_ContactIndex(i) <= 2);
        SceneActorChase_SetControl(p, ActorPhase_UpdateReverseFlicker, 0x2F);
    }
}
