#include "global.h"
#include "contact_collider.h"

struct BossContact {
    u8 unknown0[0xBC];
    u32 rewardMask, hitAttributes;
    u8 unknownC4[0xE2 - 0xC4];
    u8 damageBonus;
};
struct ImpactActor {
    struct BossContact base;
    u8 unknownE4[0x46A - 0xE4];
    u16 damage;
    u32 hitAttributes;
    u8 hitByLight, hitDirection, hitParameter;
    u8 unknown473[0x4EE - 0x473];
    u16 response;
};
void Collision_CalculateDamageResponse(struct ContactCollider *, struct ContactCollider *);
void PlayerStats_AddContactHundredths(struct ContactCollider *, struct ContactCollider *, s32);
s32 Math_Atan2(s32, s32);
static inline void SetRewardMask(struct BossContact *boss, u32 value)
{
    boss->rewardMask = value;
}
static inline void SetHitAttributes(struct BossContact *boss, u32 value)
{
    boss->hitAttributes = value;
}
void ActorImpact_CopyContact(struct ContactCollider *attack, struct ContactCollider *defense,
                             struct ImpactActor *actor)
{
    struct BossContact *boss = &actor->base;
    u32 rewardMask;
    u32 attributes;
    u16 angleFlag;
    Collision_CalculateDamageResponse(attack, defense);
    actor->damage = defense->cost;
    angleFlag = 0x100;
    if (attack->flags & angleFlag)
        actor->hitDirection = attack->direction;
    else
        actor->hitDirection = Math_Atan2(defense->x - attack->x, defense->z - attack->z);
    actor->hitParameter = attack->responseParameter;
    rewardMask = attack->statRewardFlags;
    SetRewardMask(boss, rewardMask);
    attributes = attack->reactionFlags;
    SetHitAttributes(boss, attributes);
    PlayerStats_AddContactHundredths(attack, defense, boss->damageBonus);
    {
        u32 one = 1;
        if (attack->reactionFlags & one)
            actor->hitByLight = one;
    }
    attributes = attack->reactionFlags;
    actor->hitAttributes = attributes;
    actor->response = attack->responseValue;
    PlayerStats_AddContactHundredths(attack, defense, 12);
}
