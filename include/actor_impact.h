#ifndef GUARD_ACTOR_IMPACT_H
#define GUARD_ACTOR_IMPACT_H

#include "contact_actor.h"
#include "scene_actor_components.h"

/* Shared contact/resource state used by the scene actor helpers. Names tentative. */
struct ActorImpact {
    u8 unknown0[0x48];
    struct SceneActorBody body;
    u8 unknown90[4];
    s16 currentResource;
    u8 unknown96[0x26];
    u32 rewardMask, contactAttributes;
    u8 unknownC4[8];
    u8 direction;
    u8 unknownCD[0x15];
    u8 damageBonus;
    u8 unknownE3[0x95];
    struct ContactCollider attackCollider, bodyCollider, proximityCollider;
    u16 resourceLimits[2];
    u8 unknown278[0x74];
    struct ContactPosition colliderPosition;
    u8 unknown2F4[0x170];
    u32 rewardFlags;
    u8 unknown468[2];
    u16 damage;
    u32 hitAttributes;
    u8 hitByLight, hitDirection, hitParameter;
    u8 unknown473[3];
    u8 frontalImpact;
    u8 unknown477[0x73];
    u8 resourceSelection;
    u8 unknown4EB[3];
    u16 response;
};

union ActorImpactVector {
    struct ContactPackedVector packed;
    u32 words[2];
};

void CollisionReaction_RecordImpact(struct ContactCollider *, struct ContactCollider *,
                                    struct ActorImpact *);
void ActorImpact_CopyContact(struct ContactCollider *, struct ContactCollider *, struct ActorImpact *);
void ActorImpact_NoOp(void);

#endif
