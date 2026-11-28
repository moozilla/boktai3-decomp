#include "global.h"
#include "bosses/count/count_graphics.h"
#include "scene/collision/contact_actor.h"
#include "engine/video/render.h"
#include "engine/engine_globals.h"
#include "player/player.h"
#include "player/player_slot_status.h"
#include "engine/ram.h"
#include "debug.h"
#include "engine/math/hit_spark_random.h"
#include "bosses/state_actor_action_entry.h"
#include "scene/collision/contact_position.h"
#include "scene/collision/contact_reaction_vector_layout.h"
#include "scene/collision/packed_contact_vector.h"
#include "scene/actor/scene_event_types.h"

int SceneActorGraphics_GetResourceId(struct StateActor *p)
{
    // concession-ok(duplicate-branches): every lock state names resource 0x335; the ROM still loads it
    switch (p->locked) {
        case 0:
            return 0x335;
        case 1:
            return 0x335;
        default:
            return 0x335;
    }
}

u8 *SceneActorGraphics_GetDefaultResourceId(void)
{
    return (u8 *)0x00000335;
}

int SceneActorGraphics_GetResourceId(struct StateActor *);

void CpuSet(const void *, void *, u32);

static inline void SceneActorGraphics_SelectPalette(struct SceneGraphicsActor *actor, s32 bank)
{
    if (bank == 0)
        actor->animation.palette = (u16 *)actor->graphics.paletteBank1;
    else if (bank == 1)
        actor->animation.palette = (u16 *)actor->graphics.paletteBank2;
    else {
        CpuSet(gPaletteBankData + bank * 32, actor->graphics.paletteBank0, 0x04000008);
        actor->animation.palette = (u16 *)actor->graphics.paletteBank0;
    }
}

void SceneActorGraphics_SelectResource(struct SceneGraphicsActor *actor)
{
    if (actor->graphics.effectMode) {
        actor->graphics.effectScale = 0;
        actor->graphics.effectMode = 0;
        actor->graphics.resourceId =
            SceneActorGraphics_GetResourceId((struct StateActor *)actor);
        SceneActorGraphics_SelectPalette(actor,
                                         SceneActorGraphics_GetResourceId((struct StateActor *)actor));
    }
}
void Collision_CalculateDamageResponse(void *, void *);
void PlayerStats_AddContactHundredths(void *, void *, s32);
void Sound_PlaySongChecked(u32);
void DamageNumberDisplay_Show(void *, u32, u32, void *, void *, u32);
s32 Math_Atan2(s32, s32);
s32 WirelessGame_TryAddCyclingItem(void *);
void WirelessGame_AddItem(void *, s32);

void SceneActorGraphics_OnDamageContact(struct ContactCollider *source,
                                        struct ContactCollider *contact,
                                        void *owner)
{
    struct SceneGraphicsActor *actor = owner;
    struct SceneGraphicsActor *victim = owner;
    struct SceneActorBody *body = &actor->body;
    u32 mode;
    u32 flags;
    struct ContactReactionVector size;
    if (actor->graphics.hitImmunityFrames)
        return;
    mode = gPlayers[0]->actionMode;
    Collision_CalculateDamageResponse(source, contact);
    if (actor->graphics.stepTableKind == 1 && mode != 2)
        contact->cost = 0;
    if (!(gEngineFlags & 0x1000))
        PlayerStats_AddContactHundredths(source, contact, actor->graphics.contactStatHundredths);
    actor->pendingDamage = contact->cost;
    if (contact->cost == 0) {
        Sound_PlaySongChecked(0x211);
        actor->graphics.paletteFlashFrames = 10;
        actor->animation.palette = (u16 *)actor->graphics.paletteBank1;
        size.x = 16;
        size.y = 0;
        size.z = 16;
        DamageNumberDisplay_Show(contact, contact->cost, 0, &body->position, &size, 1);
    }
    if (ContactCollider_TestFlags(source, 0x100))
        actor->hitAngle = source->direction;
    else
        actor->hitAngle = Math_Atan2(contact->x - source->x, contact->z - source->z);
    flags = source->statRewardFlags;
    victim->hitStatRewardFlags = flags;
    flags = source->reactionFlags;
    victim->hitReactionFlags = flags;
    actor->graphics.hitImmunityFrames = source->responseValue;
    PlayerStats_AddContactHundredths(source, contact, victim->contactHundredths);
    if (actor->pendingDamage > 0 && ContactCollider_TestReactionFlags(source, 1))
        actor->damagePending = 1;
    if (!(gEngineFlags & 0x1000))
        return;
    if (mode == 1 || mode == 2) {
        if (WirelessGame_TryAddCyclingItem(&contact->position) && (contact->costFlags & 8))
            WirelessGame_AddItem(&contact->position, 0);
        return;
    }
    if (contact->costFlags & 8)
        WirelessGame_AddItem(&contact->position, 0);
    if (!ContactCollider_TestReactionFlags(source, 0x800000))
        return;
    switch ((s16)(*(gGameState->slots88 + gGameState->selected) - 6)) {
        case 0:
            WirelessGame_AddItem(&contact->position, 2);
            break;
        case 1:
            WirelessGame_AddItem(&contact->position, 1);
            break;
        case 2:
            WirelessGame_AddItem(&contact->position, 3);
            break;
        case 3:
            WirelessGame_AddItem(&contact->position, 2);
            WirelessGame_AddItem(&contact->position, 1);
            break;
        case 4:
            WirelessGame_AddItem(&contact->position, 2);
            WirelessGame_AddItem(&contact->position, 3);
            break;
        case 5:
            WirelessGame_AddItem(&contact->position, 2);
            WirelessGame_AddItem(&contact->position, 1);
            WirelessGame_AddItem(&contact->position, 3);
            break;
    }
}
void SceneActorGraphics_InitBodyNoOp(void)
{
}

typedef struct ContactPosition Vec;

void SceneActorGraphics_InitBodyNoOp(void);

void SceneActorGraphics_InitializeBody(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    Vec scale, rotation;
    void *drawable = p->bodyDrawable;
    int flags;
    scale.x = 0x40;
    scale.y = 0x80;
    scale.z = 0x40;
    rotation.x = 0;
    rotation.y = 0x80;
    rotation.z = 0;
    Collider_Init(drawable, 0, 0x2001, 0, 16, &scale, &rotation);
    Collider_SetPosition(drawable, &p->worldPosition, 0);
    Collider_SetCallback(drawable, SceneActorGraphics_InitBodyNoOp, p);
    if (gEngineFlags & 0x1000)
        flags = 0x40800;
    else
        flags = 0;
    Collider_SetAttackProperties(drawable, actor->graphics.attackWidth0,
                                 actor->graphics.attackHeight0, 0, flags,
                                 actor->graphics.attackDepth0);
}
void SceneActorGraphics_OnDamageContact(struct ContactCollider *, struct ContactCollider *,
                                        void *);

void SceneActorGraphics_InitializeOverlay(struct StateActor *p, int resource)
{
    Vec scale, rotation;
    void *drawable = p->overlayDrawable;
    scale.x = 0x60;
    scale.y = 0x80;
    scale.z = 0x60;
    rotation.x = 0;
    rotation.y = 0x80;
    rotation.z = 0;
    Collider_Init(drawable, 0, 0x5101, 0, 16, &scale, &rotation);
    Collider_SetPosition(drawable, &p->worldPosition, 0);
    Collider_SetCallback(drawable, SceneActorGraphics_OnDamageContact, p);
    Collider_SetBodyProperties(drawable, resource, 0x802, 1);
    Collider_Refresh(drawable);
}

void SceneActorGraphics_UpdateVariant(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    struct ContactCollider *drawable = (struct ContactCollider *)p->bodyDrawable;
    if (SceneActorState_TestFlags(p, 0x40000))
        actor->graphics.variantToggle = 1;
    else
        actor->graphics.variantToggle = 0;
    if (actor->graphics.variantToggle != actor->graphics.variantTogglePrev) {
        switch (actor->graphics.variantToggle) {
            case 0:
                Collider_SetAttackProperties(drawable, actor->graphics.attackWidth0,
                                             actor->graphics.attackHeight0, 0, 0,
                                             actor->graphics.attackDepth0);
                break;
            case 1:
                Collider_SetAttackProperties(drawable, actor->graphics.attackWidth1,
                                             actor->graphics.attackHeight1, 0, 0x60000,
                                             actor->graphics.attackDepth1);
                break;
        }
        actor->graphics.variantTogglePrev = actor->graphics.variantToggle;
    }
}

void SceneActorGraphics_OscillateScale(struct SceneGraphicsActor *actor)
{
    actor->graphics.scale += actor->graphics.scaleStep;
    if (actor->graphics.scaleStep > 0) {
        if (actor->graphics.scale >= actor->graphics.scaleMax) {
            actor->graphics.scale = actor->graphics.scaleMax;
            actor->graphics.scaleStep = -32;
        }
    } else {
        if (actor->graphics.scale <= actor->graphics.scaleMin) {
            actor->graphics.scale = actor->graphics.scaleMin;
            actor->graphics.scaleStep = 32;
        }
    }
}

s32 SceneActorGraphics_GetEffectState(struct StateActor *p)
{
    s32 r;
    if (gPlayerEffectPendingCount != 0) {
        r = 4;
    } else if (AreaEffect_IsPositionCovered(&p->target) == 0) {
        r = 0;
    } else {
        r = gGameState->unk876;
    }
    return r;
}

void SceneActorState_SetValueClampedWhenInactive(struct StateActor *p, u32 v)
{
    if (p->variant == 0)
        v = 3;
    p->actionAnimation = v;
}

void SceneActorState_SetValueClampedWhenActive(struct StateActor *p, u32 v)
{
    if (p->variant == 1)
        v = 3;
    p->actionAnimation = v;
}

void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);

void SceneActors_StartDisappearEffect(struct StateActorPos *position)
{
    struct StateActor *p = SceneActors_GetBySlot(0);
    if (p) {
        struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
        int sequence;
        SceneActorState_StartAction(p, 0, 0, 13);
        p->target = *position;
        SceneActors_SetFacing(p, p->requestedAngle);
        sequence = (actor->graphics.animationFlags & 1) ? 24 : 62;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, &actor->animationResource,
                                              sequence, 1, 1, 6);
        Sound_PlaySongChecked(839);
        Sound_PlaySongChecked(1335);
    }
}
void ContactActorPool_RequestClear(struct ContactActorPool *);

void SceneActors_RequestContactPoolClear(void)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)SceneActors_GetBySlot(0);
    if (actor && actor->graphics.contactPool)
        ContactActorPool_RequestClear(actor->graphics.contactPool);
}
void SceneActors_RequestContactPoolClear(void);

void SceneActors_AddHealthCost(int amount)
{
    struct StateActor *p;
    struct SceneGraphicsActor *actor;
    int remaining, diff;
    if (amount <= 0)
        return;
    p = SceneActors_GetBySlot(0);
    if (!p)
        return;
    actor = (struct SceneGraphicsActor *)p;
    actor->graphics.healthCostAccumulator += amount;
    remaining = p->health - actor->graphics.healthCostAccumulator;
    if (remaining > 1) {
        diff = remaining - actor->graphics.healthCostBaseline;
        if (diff < 0)
            diff = -diff;
        if (diff < 50)
            return;
    }
    SceneActors_RequestContactPoolClear();
}void SceneActors_AddResourceCost(int amount)
{
    struct SceneGraphicsActor *actor;
    if (amount > 0) {
        actor = (struct SceneGraphicsActor *)SceneActors_GetBySlot(0);
        if (actor)
            actor->graphics.resourceCostAccumulator += amount;
    }
}
// confidence=low: index of the first of the count tracked positions at 0x110 that lies inside
// the box min..max on x and z, or -1.
struct BoxSearchPosition {
    s16 x, y, z;
};

static inline s32 SceneGraphicsActor_GetPlayerPosition(struct SceneGraphicsActor *actor,
                                                       s32 index,
                                                       struct BoxSearchPosition **out)
{
    *out = (struct BoxSearchPosition *)actor->playerPositions[index];
    return *out != NULL;
}

s32 sub_08022624(void *owner, s32 count, struct BoxSearchPosition *min,
                 struct BoxSearchPosition *max)
{
    struct SceneGraphicsActor *actor = owner;
    s32 i;
    for (i = 0; i < count; i++) {
        struct BoxSearchPosition *pos;
        SceneGraphicsActor_GetPlayerPosition(actor, i, &pos);
        if (pos) {
            s32 inside;
            if (pos->x >= min->x && pos->x <= max->x && pos->z >= min->z && pos->z <= max->z)
                inside = 1;
            else
                inside = 0;
            if (inside)
                return i;
        }
    }
    return -1;
}typedef struct ParticleOffset {
    s16 x, y, z, pad;
} ParticleOffset;
s32 SceneParticle_CreateParticles(const void *anchor, const ParticleOffset *offset,
                                  const void *range, const void *jitter, s32 count, u32 priority);

// confidence=low: emits four particles from the body position around a facing-dependent
// center, each offset by a random -64..63 in x and z.
void sub_08022694(void *owner)
{
    struct StateActor *p = owner;
    struct SceneActorBody *body = &((struct SceneGraphicsActor *)owner)->body;
    ParticleOffset center;
    ParticleOffset offset;
    s32 i;
    if (p->animationFacing4 <= 1) {
        center.x = 0x40;
        center.y = 0xc8;
        center.z = -0x80;
    } else {
        center.x = -0x80;
        center.y = 0xc8;
        center.z = 0x40;
    }
    for (i = 0; i < 4; i++) {
        const u16 *table = gRandomTable;
        u16 index = (gRandomIndex + 1) & 1023;
        u16 random = table[index];
        offset.x = (random & 0x7F) + center.x - 0x40;
        offset.y = center.y;
        index = (index + 1) & 1023;
        gRandomIndex = index;
        random = table[index];
        offset.z = (random & 0x7F) + center.z - 0x40;
        table = NULL;
        SceneParticle_CreateParticles(&body->position, &offset, table, table, 1, 1);
    }
}
// confidence=low: five sparks 300 units above the actor; the large burst for big, otherwise a
// smaller one shifted by the facing kept at 0xA8.
void sub_08022790(void *owner, s32 big)
{
    struct StateActor *p = owner;
    struct ContactPosition pos;
    struct PackedContactVector size;
    s32 angle, count, speed;
    u32 angleIndex, countIndex, speedIndex;
    const u16 *table;
    struct ContactPosition *at = &pos;
    pos = *(struct ContactPosition *)&p->worldPosition;
    at->y += 300;
    if (big) {
        size.x = 164;
        size.y = 32;
        size.z = 164;
    } else {
        switch (p->animationFacing4) {
            case 1:
                pos.x -= 0x80;
                break;
            case 2:
                at->z -= 0x80;
                break;
        }
        size.x = 128;
        size.y = 16;
        size.z = 128;
    }
    table = gRandomTable;
    angleIndex = (gRandomIndex + 1) & 1023;
    angle = (u8)table[angleIndex];
    countIndex = (angleIndex + 1) & 1023;
    count = (table[countIndex] % 4) + 2;
    speedIndex = (countIndex + 1) & 1023;
    gRandomIndex = speedIndex;
    speed = (table[speedIndex] % 4) + 20;
    SparkParticle_Spawn(&pos, 5, &size, angle, count, speed, 0, 2);
}
typedef struct PackedContactVector BurstVec;
extern const BurstVec gUnk_08604D84[];
u16 Sqrt(u32);
s32 Div(s32, s32);
void ImpactBurst_EmitBurst(void *, s32, s32, s32, s32, s32, s32, s32);
void TransientSpriteEffects_Spawn(int, int, void *, void *, int, int);

/* Emit particles away from a point above the center of the burst. */
static inline void BurstNormalize(BurstVec *v, s32 speed)
{
    u16 len = Sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    if (len) {
        v->x = Div(v->x * speed, len);
        v->y = Div(v->y * speed, len);
        v->z = Div(v->z * speed, len);
    }
}

static inline s32 BurstRandom(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}

void sub_08022878(BurstVec *p)
{
    BurstVec center = *p;
    s32 i;
    BurstVec pos, vel;
    center.y += 0xc8;
    for (i = 0; i < 8; i++) {
        void *data = (void *)gRandomTable;
        pos.x = (s16)(p->x + gUnk_08604D84[i].x - 16) + (BurstRandom(data) & 0x1f);
        pos.y = (s16)(p->y + gUnk_08604D84[i].y - 16) + (BurstRandom(data) & 0x1f);
        pos.z = (s16)(p->z + gUnk_08604D84[i].z - 16) + (BurstRandom(data) & 0x1f);
        data = &pos;
        ImpactBurst_EmitBurst(data, 1, 0, 0x40, 0xc, 0xc, 4, 4);
        vel.x = pos.x - center.x;
        vel.y = pos.y - center.y;
        vel.z = pos.z - center.z;
        BurstNormalize(&vel, 5);
        TransientSpriteEffects_Spawn(2, 0, &pos, &vel, 0x20, 0x100);
    }
}

void SceneActors_SpawnAmbientParticlePair(struct StateActor *p)
{
    struct AnimSprite *animation = &((struct SceneGraphicsActor *)p)->animation;
    struct PackedContactVector size;
    int i;
    for (i = 0; i < 2; i++) {
        u32 angle, count, speed;
        u32 angleIndex, countIndex, speedIndex;
        u32 table;
        size.x = 164;
        size.y = 16;
        size.z = 164;
        table = (u32)gRandomTable;
        angleIndex = HitSpark_NextIndex(gRandomIndex, 1023);
        angle = (u8)HitSpark_Sample(table, angleIndex);
        countIndex = HitSpark_NextIndex(angleIndex, 1023);
        count = (HitSpark_Sample(table, countIndex) % 4) + 2;
        speedIndex = HitSpark_NextIndex(countIndex, 1023);
        gRandomIndex = speedIndex;
        speed = (HitSpark_Sample(table, speedIndex) % 4) + 20;
        table = 3;
        SparkParticle_Spawn(&animation->position, table, &size, angle, count, speed, 2, 2);
    }
}
void SceneActors_StopBackgroundFlash(struct SceneGraphicsActor *actor)
{
    BackgroundFlash_BeginDisappearance(actor->graphics.backgroundFlash);
    actor->graphics.backgroundFlashFlag = 0;
}
void SceneActors_UpdateShrinkingReaction(void *, int);
void SceneActors_UpdateKnockbackReaction(void *, int);
void sub_08027214(void *, int);
void WirelessGame_AddItem(void *, s32);

// confidence=low: damage handler: 0 shows the damage number and starts the shrinking reaction
// (wireless games may award an item when hit from the front), 2 starts a knockback, 1 a death.
s32 sub_08022B18(struct StateActor *p, s32 kind, s32 damage)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct SceneActorBody *body = &((struct SceneGraphicsActor *)p)->body;
    s32 result = 1;
    switch (kind) {
        case 0: {
            s32 shown = damage;
            if (((struct SceneGraphicsActor *)p)->graphics.stepTableKind == 1 && gPlayers[0]->actionMode != 2) {
                shown = 0;
                result = 0;
            }
            if (damage > 0) {
                struct ContactReactionVector size;
                size.x = 0x20;
                size.y = 0;
                size.z = 0x20;
                DamageNumberDisplay_Show(&actor->overlayCollider, shown, 0, &body->position, &size,
                                         1);
            }
            if (SceneActorState_TestFlags(p, 0x100000))
                SceneActors_StopBackgroundFlash(actor);
            SceneActorState_StartAction(p, 1, 0, 0);
            SceneActorState_EnterAction(p, SceneActors_UpdateShrinkingReaction, 0x16);
            if (gEngineFlags & 0x1000) {
                if (WirelessGame_TryAddCyclingItem(&body->position)) {
                    s32 difference = actor->overlayCollider.direction -
                                     Math_Atan2(body->position.x - gGameState->positionX,
                                                body->position.z - gGameState->positionZ);
                    if (difference < 0)
                        difference = -difference;
                    if (difference <= 0x20)
                        WirelessGame_AddItem(&body->position, 0);
                }
            }
            break;
        }
        case 2:
            SceneActorState_StartAction(p, 1, 0, 1);
            SceneActorState_EnterAction(p, SceneActors_UpdateKnockbackReaction, 0x17);
            ((struct SceneGraphicsActor *)p)->hitAngle = body->direction + 0x80;
            break;
        case 1:
            ((struct SceneGraphicsActor *)p)->hitAngle = actor->damageSource[0x2D];
            SceneActorState_SetFlags(p, 2);
            SceneActorState_StartAction(p, 1, 0, 3);
            SceneActorState_EnterAction(p, sub_08027214, 0x19);
            break;
    }
    return result;
}
void SceneActorState_StepTableByte(struct StateActor *p, const u8 *tbl)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    actor->graphics.stepTableValuePrev = actor->graphics.stepTableValue;
    actor->graphics.stepTableValue =
        *(tbl + actor->graphics.stepTableRow * 8 + actor->graphics.stepTableCol);
    actor->graphics.stepTableCol++;
    if (actor->graphics.stepTableCol > 7)
        actor->graphics.stepTableCol = 0;
}
extern const u8 gUnk_08604DC4[], gUnk_08604DE4[], gUnk_08604E04[], gUnk_08604E24[];

void SceneActorState_StepTableByte(struct StateActor *p, const u8 *tbl);

int SceneActorState_StepTableByKind(struct StateActor *p)
{
    switch (((struct SceneGraphicsActor *)p)->graphics.stepTableKind) {
        case 0:
            SceneActorState_StepTableByte(p, gUnk_08604DC4);
            return 1;
        case 1:
            SceneActorState_StepTableByte(p, gUnk_08604DE4);
            return 1;
        case 2:
            SceneActorState_StepTableByte(p, gUnk_08604E04);
            return 1;
        case 3:
            SceneActorState_StepTableByte(p, gUnk_08604E24);
            return 1;
    }
    return 0;
}
// confidence=low: chooses the next action phase from the reaction kind and flash state.
static inline s32 SceneActorState_GetHealthRatio(const struct StateActor *actor)
{
    s32 ratio = actor->healthRatio;
    return ratio;
}

void SceneActorState_SelectReactionPhase(struct SceneGraphicsActor *actor)
{
    struct StateActor *state = (struct StateActor *)actor;
    if (actor->graphics.stepTableKind == 3) {
        if (actor->graphics.flashPhase) {
            SceneActorState_EnterPhase(state, 14);
            return;
        }
    } else {
        if (actor->graphics.flashPhase == 3)
            SceneActors_StopBackgroundFlash(actor);
        else if (actor->graphics.flashPhase) {
            SceneActorState_EnterPhase(state, 14);
            return;
        }
    }
    switch (actor->graphics.stepTableValue) {
        case 0:
            SceneActorState_EnterPhase(state, 9);
            return;
        case 1:
            SceneActorState_EnterPhase(state, 10);
            return;
        case 2:
            if (actor->graphics.stepTableKind != 3 && SceneActorState_GetHealthRatio(state) <= 63)
                actor->graphics.particleFlag = 1;
            else
                actor->graphics.particleFlag = 0;
            SceneActorState_EnterPhase(state, 2);
            return;
        case 3:
            SceneActorState_EnterPhase(state, 11);
            return;
        case 4:
            if (SceneActorState_GetHealthRatio(state) <= 63) {
                SceneActorState_EnterPhase(state, 14);
                return;
            } else {
                SceneActorState_EnterPhase(state, 11);
                return;
            }
        case 5:
            SceneActorState_EnterPhase(state, 15);
            return;
        case 6:
            SceneActorState_EnterPhase(state, 16);
            return;
    }
}
// confidence=low: reaction-kind selector controls the far-point mode and motion timer.
void Path_SnapToRandomFarPoint(void *, void *, u32, u32);
void SceneActorState_SnapReactionPosition(struct SceneGraphicsActor *actor,
                                          struct ContactPosition *position)
{
    struct SceneActorBody *body = &actor->body;
    switch (actor->graphics.stepTableValue) {
        case 0:
            Path_SnapToRandomFarPoint(position, &body->position, 0, 65536);
            actor->graphics.approachFrames = 64;
            return;
        case 1:
            Path_SnapToRandomFarPoint(position, &body->position, 2, 65536);
            actor->graphics.approachFrames = 64;
            return;
        case 2:
            Path_SnapToRandomFarPoint(position, &body->position, 1, 65536);
            actor->graphics.approachFrames = 32;
            return;
        case 3:
            Path_SnapToRandomFarPoint(position, &body->position, 3, 65536);
            actor->graphics.approachFrames = 64;
            return;
        case 4:
            Path_SnapToRandomFarPoint(position, &body->position, 3, 65536);
            actor->graphics.approachFrames = 64;
            return;
        case 5:
            Path_SnapToRandomFarPoint(position, &body->position, 3, 65536);
            actor->graphics.approachFrames = 64;
            return;
        case 6:
            Path_SnapToRandomFarPoint(position, &body->position, 2, 65536);
            actor->graphics.approachFrames = 64;
            return;
        default:
            *position = *(struct ContactPosition *)&((struct StateActor *)actor)->target;
            break;
    }
    actor->graphics.approachFrames = 64;
}
/* Scene commands set actor states, positions, graphics variants and background flashes. */
/* 0x0200047C: the dispatch phase selects the pending command list. */
struct SceneEventDispatchState {
    u8 unknown0[0x24];
    u8 phase;
};
extern struct SceneEventDispatchState *gSceneEventDispatchState;

static inline void SceneActorGraphics_SetEffect(struct SceneGraphicsActor *actor, int mode,
                                                int resource, int duration)
{
    actor->graphics.resourceId = resource;
    actor->graphics.effectMode = mode;
    actor->graphics.effectDuration = duration;
    if (actor->graphics.effectMode == 2) {
        u32 scale = duration == 5 ? 32 : 64;
        actor->graphics.effectScale = scale;
    } else if (actor->graphics.effectMode == 1)
        actor->graphics.effectScale = 0;
}

void SceneActors_DispatchCommands(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct SceneEventReceiver *receiver = (struct SceneEventReceiver *)p->receiver2;
    s32 i;

    i = 0;
    if (i < receiver->count[gSceneEventDispatchState->phase]) do {
        struct SceneEventDispatchState *dispatch = gSceneEventDispatchState;
        struct SceneGraphicsCommand *command =
            (struct SceneGraphicsCommand *)receiver->pending[dispatch->phase][i];
        SceneMessages_SetPendingTrigger(receiver, command);
        switch (command->kind) {
            case 0:
                    SceneActorState_StartAction(p, 2, 0, 0);
                    break;
            case 1:
                    p->savedFacing = (command->arguments[0] + 5) << 5;
                    SceneActorState_StartAction(p, 2, 0, 1);
                    break;
            case 3:
                    p->savedFacing = (command->arguments[0] + 5) << 5;
                    SceneActorState_StartAction(p, 2, 0, 2);
                    break;
            case 4:
                    p->savedFacing = (command->arguments[0] + 5) << 5;
                    SceneActorState_StartAction(p, 2, 0, 3);
                    break;
            case 5:
                    p->savedFacing = (command->arguments[0] + 5) << 5;
                    SceneActorState_StartAction(p, 2, 0, 4);
                    break;
            case 6:
                    p->savedFacing = (command->arguments[0] + 5) << 5;
                    SceneActorState_StartAction(p, 2, 0, 5);
                    break;
            case 7:
                    SceneActorState_StartAction(p, 2, 0, 6);
                    break;
            case 8:
                    SceneActorState_StartAction(p, 2, 0, 7);
                    break;
            case 10:
                    SceneActorState_StartAction(p, 2, 0, 8);
                    break;
            case 11:
                    SceneActorState_StartAction(p, 2, 0, 9);
                    break;
            case 12:
                    actor->graphics.snapPosition.x = command->arguments[0];
                    actor->graphics.snapPosition.y = command->arguments[1];
                    actor->graphics.snapPosition.z = command->arguments[2];
                    SceneActorState_StartAction(p, 2, 0, 10);
                    break;
            case 19:
                    SceneActorState_StartAction(p, 2, 0, 11);
                    break;
            case 20:
                    SceneActorState_StartAction(p, 2, 0, 12);
                    break;
            case 21:
                    SceneActorState_StartAction(p, 2, 0, 13);
                    break;
            case 23:
                    SceneActorState_StartAction(p, 2, 0, 14);
                    break;
            case 24:
                    SceneActorState_StartAction(p, 2, 0, 15);
                    break;
            case 25:
                    if (command->arguments[0])
                        SceneActors_StopBackgroundFlash(actor);
                    else if (command->arguments[1])
                        BackgroundFlash_BeginAlternating(actor->graphics.backgroundFlash);
                    else
                        BackgroundFlash_BeginAppearance(actor->graphics.backgroundFlash);
                    break;
            case 13:
                    SceneActorState_StartAction(p, 2, 0, 16);
                    break;
            case 14:
                    SceneActorState_StartAction(p, 2, 0, 18);
                    break;
            case 22:
                    p->targetPosition.x = command->arguments[0];
                    p->targetPosition.y = command->arguments[1];
                    p->targetPosition.z = command->arguments[2];
                    SceneActorState_StartAction(p, 2, 0, 19);
                    break;
            case 18:
                    SceneActorState_StartAction(p, 2, 0, 20);
                    break;
            case 15:
                    if (command->arguments[0] == 0) {
                        actor->graphics.effectScale = 0;
                        actor->graphics.effectMode = 0;
                        actor->graphics.resourceId =
                            SceneActorGraphics_GetResourceId((struct StateActor *)actor);
                        SceneActorGraphics_SelectPalette(
                            actor, SceneActorGraphics_GetResourceId((struct StateActor *)actor));
                    } else if (command->arguments[1] == 0) {
                        SceneActorGraphics_SetEffect(actor, 3, 0x338, 5);
                    } else {
                        SceneActorGraphics_SetEffect(actor, 3, 0x33A, 5);
                    }
                    Trigger_ConsumePending(p->receiver2, 1);
                    break;
            case 16:
                    if (command->arguments[0] == 0)
                        SceneActorGraphics_SetEffect(actor, 1, 0x335, 5);
                    else if (command->arguments[1] == 0)
                        SceneActorGraphics_SetEffect(actor, 2, 0x338, 5);
                    else
                        SceneActorGraphics_SetEffect(actor, 2, 0x33A, 5);
                    Trigger_ConsumePending(p->receiver2, 1);
                    break;
            case 9:
                    SceneActors_ConfigureEffect(p, command->arguments[0], 0xB4, 0xB4);
                    Trigger_ConsumePending(p->receiver2, 1);
                    break;
            case 17:
                    actor->graphics.unk3B8 = command->arguments[0];
                    Trigger_ConsumePending(p->receiver2, 1);
                    break;
            case 2:
                    p->target.x = command->arguments[0];
                    p->target.y = command->arguments[1];
                    p->target.z = command->arguments[2];
                    actor->animation.position.x = p->target.x;
                    actor->animation.position.y = p->target.y + actor->graphics.scale;
                    actor->animation.position.z = p->target.z;
                    Trigger_ConsumePending(p->receiver2, 1);
                    break;
            case 66:
                    actor->graphics.projectileDirection = command->arguments[0];
                    SceneActorState_StartAction(p, 2, 0, 21);
                    break;
        }
        i++;
    } while (i < receiver->count[gSceneEventDispatchState->phase]);
}

void SceneActorState_AdvanceToState1A(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x1a);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}

void SceneActorState_AdvanceToState01(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p) == TRUE) {
        SceneActorState_SetAnimation(p, 1);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}
void SceneActorState_AdvanceToState02(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 2);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}

void SceneActorState_Acknowledge(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 3);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_AdvanceToState1B(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x1b);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_AdvanceToState1C(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x1C);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_Resume(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x1d);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    if (p->actionFinished) {
        SceneActorState_StartAction(p, 2, 0, 1);
        Trigger_ConsumePending(p->receiver2, 1);
    } else
        p->actionTimer++;
}

void SceneActorState_AdvanceToState1E(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x1e);
    if (p->actionFinished) {
        SceneActorState_StartAction(p, 2, 0, 0);
        Trigger_ConsumePending(p->receiver2, 1);
    } else
        p->actionTimer++;
}

void SceneActorState_AdvanceToState23(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x23);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_AdvanceToState24(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x24);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_AdvanceToState06(struct StateActor *p)
{
    struct SceneGraphicsActor *graphicsActor = (struct SceneGraphicsActor *)p;
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 6);
        graphicsActor->graphics.particleFlag = 0;
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_AdvanceToState20(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x20);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_AdvanceToState21(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x21);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_Hold(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x22);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    if (!p->actionFinished)
        p->actionTimer++;
}

void SceneActorState_PlayAnimation17AndConsumeTrigger(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x11);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_PlayAnimation19AndConsumeTrigger(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x13);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_PlayAnimation14ThenAdvance(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0xe);
    if (p->actionFinished) {
        SceneActorState_EnterPhase(p, 0x11);
        Trigger_ConsumePending(p->receiver2, 1);
    } else
        p->actionTimer++;
}

void SceneActorState_PlayAnimation15AndConsumeTrigger(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0xf);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}

void SceneActorState_ResumeAfterAction(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x10);
    if (p->actionFinished) {
        SceneActorState_StartAction(p, 2, 0, 1);
        Trigger_ConsumePending(p->receiver2, 1);
    } else
        p->actionTimer++;
}

void SceneActorState_PlayAnimation31AndConsumeTrigger(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x1F);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_PlayAnimation37ThenResume(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x25);
    if (p->actionFinished) {
        SceneActorState_StartAction(p, 2, 0, 0);
        Trigger_ConsumePending(p->receiver2, 1);
    } else
        p->actionTimer++;
}

void SceneActorState_PlayAnimation39AndConsumeTrigger(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x27);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

extern const u32 gUnk_08604E44[];

void SceneActorState_DispatchPhaseHandler(struct StateActor *p)
{
    Callback_InvokeActor(p, gUnk_08604E44[p->actionPhase]);
}

int SceneActorState_TryStartDeathOrFlaggedAction(struct StateActor *p)
{
    if (p->health <= 0 && p->vulnerable) {
        SceneActorState_SetFlags(p, 2);
        SceneActorState_EnterAction(p, sub_08027214, 25);
    }
    p->vulnerable = 0;
    if (SceneActorState_TestFlags(p, 2)) {
        SceneActorState_StartAction(p, 1, 0, 3);
        return 1;
    } else if (p->actionFlags & 1) {
        SceneActorState_StartAction(p, 1, 0, 2);
        return 1;
    }
    return 0;
}

void SceneActorState_WaitForDelayedAction(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p) == TRUE) {
        if (actor->variant == 0)
            p->actionAnimation = 1;
        else
            SceneActorState_SetAnimation(p, 2);
    }
    if (p->actionTimer > actor->graphics.recoveryTimer && SceneActorState_StepTableByKind((struct StateActor *)actor)) {
        SceneActorState_SnapReactionPosition(actor, &actor->graphics.snapPosition);
        SceneActorState_EnterPhase(p, 1);
    } else
        p->actionTimer++;
}
void SceneActorState_UpdateVariantActionPlayback(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetValueClampedWhenInactive(actor, 4);
    if (p->actionFinished) {
        if (((struct StateActor *)actor)->actionState != 3) {
            SceneActorState_SelectReactionPhase((u8 *)actor);
            actor->graphics.recoveryTimer = 60;
            return;
        } else
            SceneActorState_SetAnimation((struct StateActor *)actor, 4);
    }
    p->actionTimer++;
}

void SceneActorState_PlayAnimation5ThenAdvance(struct StateActor *p)
{
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 5);
    if (p->actionFinished)
        SceneActorState_EnterPhase(p, 3);
    else
        p->actionTimer++;
}

void SceneActorState_EnterPhase6AndResetAnimation(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    SceneActorState_EnterPhase(p, 6);
    SceneActorState_SetAnimation(p, 9);
    p->actionReset(p);
    p->actionAnimation = 0;
    SceneActorState_ClearFlags(p, 0x40);
    actor->graphics.trackedPlayerEscaped = 0;
}

struct ProbeBox {
    s32 x : 16, y : 16, z : 16, w : 16;
};

struct ProbePos {
    s16 x, y, z, w;
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);

static inline s32 ProbeTruncate12(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

static inline void ProbeProject(struct ProbePos *out, u8 angle, s32 distance)
{
    out->x = ProbeTruncate12(gUnk_086149C4[(angle + 0x40) & 0xff] * distance);
    out->y = 0;
    out->z = ProbeTruncate12(gUnk_086149C4[angle] * distance);
}

static inline void ProbeAdd(struct ProbePos *dst, struct ProbePos *src)
{
    dst->x += src->x;
    dst->y += src->y;
    dst->z += src->z;
}

static inline s32 ProbeCellAt(struct ProbePos *p)
{
    s32 x = p->x >> 8;
    s32 z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline u8 *ProbeCellProps(s32 cell)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + (cell * 4 + 12);
}

static inline u32 SceneActorState_TestPlayerFlags(const struct Player *player, u32 flags)
{
    return player->statusFlags & flags;
}

static inline bool32 ProbeHeightOutOfRange(s32 height, s32 referenceHeight, s32 minDelta,
                                           s32 maxDelta)
{
    u32 delta = height - referenceHeight - minDelta;
    u32 span = maxDelta - minDelta;
    return delta > span;
}

static inline bool32
SceneActorGraphics_IsForwardGroundOutOfRange(const struct SceneGraphicsActor *actor, s32 distance)
{
    struct ProbePos pos;
    u8 *s = (u8 *)actor + 0x48;
    u8 *props;
    struct ProbePos *at;
    u32 type;
    s32 h;
    ProbeProject(&pos, s[9], distance);
    ProbeAdd(&pos, (struct ProbePos *)(s + 0xc));
    at = &pos;
    props = ProbeCellProps(ProbeCellAt(at));
    type = *props >> 4;
    h = (*props & 0xf) << 8;
    switch (type) {
        case 1:
            h -= (u8)at->z;
            break;
        case 2:
            h -= (u8)at->x;
            break;
    }
    return ProbeHeightOutOfRange(h, actor->graphics.recoveryGroundHeight, -0x1ff, 0xff);
}

// confidence=low: grab a player in reach, or advance when the animation ends or the
// forward terrain height is outside the permitted interval.
void sub_08023FE8(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct ProbePos *sprite = (struct ProbePos *)((u8 *)p + 0x48);
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 6);
    if (actor->graphics.particleFlag == 1) {
        struct ProbePos off, min, max;
        s32 r;
        ProbeProject(&off, ((u8 *)sprite)[9], 0x80);
        ProbeAdd(&off, (struct ProbePos *)((u8 *)sprite + 0xc));
        min.x = -0x80;
        min.z = -0x80;
        ProbeAdd(&min, &off);
        max.x = 0x80;
        max.z = 0x80;
        ProbeAdd(&max, &off);
        r = sub_08022624(actor, ((u8 *)actor)[0x174], (struct BoxSearchPosition *)&min,
                         (struct BoxSearchPosition *)&max);
        if (r >= 0) {
            struct Player *player = gPlayers[r];
            if (!SceneActorState_TestPlayerFlags(player, 0x40000)) {
                actor->graphics.trackedPlayer = r;
                SceneActorState_EnterPhase6AndResetAnimation(p);
                return;
            }
        }
    }
    if (SceneActorGraphics_IsForwardGroundOutOfRange(actor, 0x190) || p->actionFinished)
        SceneActorState_EnterPhase(p, 5);
    else
        p->actionTimer++;
}

static inline void SceneActorState_StartRecovery(struct SceneGraphicsActor *actor, u32 duration)
{
    SceneActorState_StartAction((struct StateActor *)actor, 0, 0, 0);
    if (gEngineFlags & 0x1000)
        actor->graphics.recoveryTimer = 0;
    else
        actor->graphics.recoveryTimer = duration;
}

void SceneActorState_PlayAnimation7ThenRecover(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 7);
    if (p->actionFinished)
        SceneActorState_StartRecovery(actor, 90);
    else
        p->actionTimer++;
}

void SceneActorState_PlayAnimation8ThenRecover(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 8);
    if (p->actionFinished)
        SceneActorState_StartRecovery(actor, 30);
    else
        p->actionTimer++;
}
// confidence=low: the tracked player escaping selects the next recovery phase.
void sub_080243CC(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p)) {
        DEBUG_PRINT();
    }
    if (p->actionFinished) {
        if (actor->graphics.trackedPlayerEscaped)
            SceneActorState_EnterPhase(p, 8);
        else
            SceneActorState_EnterPhase(p, 7);
    } else {
        p->actionTimer++;
    }
}

void SceneActorState_PlayAnimation10ThenRecover(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 10);
    if (p->actionFinished)
        SceneActorState_StartRecovery(actor, 30);
    else
        p->actionTimer++;
}

void SceneActorState_PlayAnimation11ThenRecover(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 11);
    if (p->actionFinished)
        SceneActorState_StartRecovery(actor, 30);
    else
        p->actionTimer++;
}

void SceneActorState_PlayVariantAnimation12ThenRecover(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetValueClampedWhenActive(actor, 12);
    if (p->actionFinished) {
        if (((struct StateActor *)actor)->actionState == 3) {
            SceneActorState_SetAnimation((struct StateActor *)actor, 12);
        } else {
            SceneActorState_StartRecovery(actor, 90);
            return;
        }
    }
    p->actionTimer++;
}
void SceneActorState_PlayVariantAnimation13ThenRecover(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetValueClampedWhenActive(actor, 13);
    if (p->actionFinished) {
        if (((struct StateActor *)actor)->actionState == 3) {
            SceneActorState_SetAnimation((struct StateActor *)actor, 13);
        } else {
            SceneActorState_StartRecovery(actor, 150);
            return;
        }
    }
    p->actionTimer++;
}void ContactActorPool_Start();

void SceneActorState_StartContactPool(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    struct SceneActorBody *body = &actor->body;
    struct ContactPosition pos;
    struct ContactPosition pos2;

    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 14);
    if (p->actionFinished) {
        struct ContactPosition *center = &pos;
        struct ContactPosition *spread;
        s32 health;
        pos = body->position;
        center->y += 200;
        spread = &pos2;
        pos2 = pos;
        health = p->health;
        actor->graphics.healthCostBaseline = health;
        if (actor->graphics.contactPool)
            ContactActorPool_Start(actor->graphics.contactPool, center, spread, 10, 10);
        SceneActorState_EnterPhase(p, 12);
    } else
        p->actionTimer++;
}
void SceneActorState_HoldAnimation15(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 15);
    p->actionTimer++;
}
void SceneActorState_UpdateLongRecovery(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 16);
    if (p->actionFinished)
        SceneActorState_StartRecovery(actor, 90);
    else
        p->actionTimer++;
}
// confidence=low: variant-dependent recovery animations and timed return to the default action.
void SceneActorState_UpdateVariantRecovery(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    if (StateActor_ConsumeActionEntry(p)) {
        if (actor->graphics.stepTableKind == 3 && actor->graphics.flashPhase)
            SceneActorState_SetValueClampedWhenActive(actor, 38);
        else
            SceneActorState_SetValueClampedWhenActive(actor, 17);
    }
    if (p->actionFinished) {
        if (((struct StateActor *)actor)->actionState == 3) {
            if (actor->graphics.stepTableKind == 3 && actor->graphics.flashPhase)
                SceneActorState_SetAnimation((struct StateActor *)actor, 38);
            else
                SceneActorState_SetAnimation((struct StateActor *)actor, 17);
        } else if (p->actionState == 17) {
            if (actor->graphics.animationFlags & 4)
                SceneActorState_SetAnimation(p, 19);
            else
                SceneActorState_SetAnimation(p, 18);
        } else {
            SceneActorState_StartRecovery(actor, 90);
            return;
        }
    }
    p->actionTimer++;
}

void SceneActorState_UpdateAction20Recovery(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetValueClampedWhenActive(actor, 20);
    if (p->actionFinished) {
        if (((struct StateActor *)actor)->actionState == 3) {
            SceneActorState_SetAnimation((struct StateActor *)actor, 20);
        } else {
            SceneActorState_StartRecovery(actor, 90);
            return;
        }
    }
    p->actionTimer++;
}
void SceneActorState_UpdateAction21Recovery(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetValueClampedWhenActive(actor, 21);
    if (p->actionFinished) {
        if (((struct StateActor *)actor)->actionState == 3) {
            SceneActorState_SetAnimation((struct StateActor *)actor, 21);
        } else {
            SceneActorState_StartRecovery(actor, 90);
            return;
        }
    }
    p->actionTimer++;
}
void SceneActorState_UpdateAction22(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 22);
    p->actionTimer++;
}
void SceneActorState_UpdateShortRecovery(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 23);
    if (p->actionFinished)
        SceneActorState_StartRecovery(actor, 30);
    p->actionTimer++;
}
void SceneActorState_WaitForFlagRelease(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 24);
    if (!SceneActorState_TestFlags(p, 1))
        SceneActorState_StartRecovery(actor, 30);
    else
        p->actionTimer++;
}

void SceneActorState_UpdatePassiveAction(struct StateActor *p)
{
    StateActor_ConsumeActionEntry(p);
    p->actionTimer++;
}

extern const u32 gUnk_08604E9C[];

void SceneActorState_DispatchPrimaryAction(struct StateActor *p)
{
    Callback_InvokeActor(p, gUnk_08604E9C[p->actionPhase]);
}

extern const u32 gUnk_08604EE0[];

void SceneActorState_DispatchSecondaryAction(struct StateActor *p)
{
    Callback_InvokeActor(p, gUnk_08604EE0[p->actionPhase]);
}

extern u32 gUnk_02000560;

extern const u32 gUnk_08604EF0[];

void SceneActorState_UpdateActiveActions(struct StateActor *p)
{
    if (!PlayerSlot_ReadStatus(0, 4)) {
        if (SceneActorState_TryStartDeathOrFlaggedAction(p) == 0) {
            Callback_InvokeActor(p, gUnk_08604EF0[p->actionIndex]);
        }
    }
}

extern const u32 gUnk_08604EF4[];

void SceneActorState_UpdateSecondaryActions(struct StateActor *p)
{
    Callback_InvokeActor(p, gUnk_08604EF4[p->actionIndex]);
}

extern const u32 gUnk_08604EF8[];

void SceneActorState_UpdatePassiveActions(struct StateActor *p)
{
    Callback_InvokeActor(p, gUnk_08604EF8[p->actionIndex]);
}

extern u32 gUnk_08604EFC[];

void SceneActorState_UpdateActionController(struct StateActor *p)
{
    /* Raw offset: 0xA1 is still unknown in state_actor.h (within unkA0). */
    ((u8 *)p)[0xa1] = 0;
    Callback_InvokeActor(p, gUnk_08604EFC[p->actionMode]);
}

void SceneActorAnimation_UpdateDirectionalCallback(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    int sequence;
    if (!p->locked)
        SceneActors_SetFacing(p, p->requestedAngle);
    if (p->actionChanged) {
        p->actionChanged = 0;
        if (p->locked)
            SceneActors_SetFacing(p, p->savedFacing);
        sequence = (actor->graphics.animationFlags & 1) ? 5 : 43;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, &actor->animationResource,
                                              sequence, 1, 1, 4);
    } else {
        sequence = (actor->graphics.animationFlags & 1) ? 5 : 43;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, &actor->animationResource,
                                              sequence, 1, 1, 1);
    }
    actor->graphics.scaleMin = 0;
    actor->graphics.scaleMax = 0;
    actor->variant = 0;
    if (!p->actionTryUpdate(p)) {
        SceneActorState_SetFlags(p, 0x40);
        p->actionReset(p);
    }
}
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);

void SceneActorAnimation_UpdateEightDirectionCallback(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    int sequence;
    if (!p->locked)
        SceneActors_SetFacing(p, p->requestedAngle);
    if (p->actionChanged) {
        p->actionChanged = 0;
        if (p->locked)
            SceneActors_SetFacing(p, p->savedFacing);
        if (actor->graphics.animationFlags & 1)
            sequence = 0;
        else
            sequence = 38;
        SceneActors_StartEightDirectionAnimation(p, &actor->animation, &actor->animationResource,
                                              sequence, 1, 1, 4);
    } else {
        if (actor->graphics.animationFlags & 1)
            sequence = 0;
        else
            sequence = 38;
        SceneActors_StartEightDirectionAnimation(p, &actor->animation, &actor->animationResource,
                                              sequence, 1, 1, 1);
    }
    if (!p->actionTryUpdate(p)) {
        SceneActorState_SetFlags(p, 0x40);
        p->actionReset(p);
    }
}