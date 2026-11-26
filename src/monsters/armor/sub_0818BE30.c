#include "monsters/armor/scene_combat_actor.h"
#include "scene/collision/contact_collider.h"
extern const u8 gUnk_0824F10C[], gUnk_0824F114[], gUnk_0824F11C[];
void Armor_InitCollider(void *, u32, void *, const void *, const void *, void *, void *);
void Armor_InitExtendedCollider(void *, u32, s32, void *, const void *, const void *, void *,
                                void *);
void Collider_SetAttackProperties(void *, s32, s32, u32, u32, s32);
void Armor_ReceiveColliderContact(void);
void Armor_TriggerBehavior35(void);
static inline void addCategory(struct ContactCollider *p, u32 mask)
{
    p->statRewardFlags |= mask;
}
void Armor_InitCombatColliders(struct SceneCombatActor *input, u32 inputId)
{
    struct ContactCollider *c_2;
    struct SceneCombatActor *p = input;
    u32 id = inputId;
    s32 value, power;
    s32 defense;
    struct ContactCollider *c;
    void *position;

    Armor_InitCollider(&p->colliderGroup.colliders[1], id, &p->position, gUnk_0824F10C, gUnk_0824F114, 0, 0);
    value = (s16)p->attack << 10;
    if (value >= 0) {
        power = value >> 12;
    } else {
        power = -(-value >> 12);
    }
    c = &p->colliderGroup.colliders[1];
    Collider_SetAttackProperties(c, power, 50, 0, 0x40000, 30);
    c->flags |= 4;
    c_2 = &p->colliderGroup.colliders[2];
    defense = (s16)p->defense;
    position = &p->position;
    Armor_InitExtendedCollider(c_2, id, defense, position, gUnk_0824F10C, gUnk_0824F114,
                               Armor_ReceiveColliderContact, p);
    ContactCollider_SetFlags(c_2, 0x100C);
    c = &p->colliderGroup.colliders[3];
    Armor_InitExtendedCollider(c, id, 0, position, gUnk_0824F10C, gUnk_0824F11C,
                               Armor_TriggerBehavior35, p);
    c->flags |= 6;
    Armor_InitCollider(&p->colliderGroup.colliders[0], id, position, gUnk_0824F10C, gUnk_0824F114, 0, p);
    value = (s16)p->attack << 11;
    if (value >= 0) {
        power = value >> 12;
    } else {
        power = -(-value >> 12);
    }
    c = &p->colliderGroup.colliders[0];
    Collider_SetAttackProperties(c, power, 50, 0x2000, 0x60000, 30);
    c->flags |= 4;
    switch (p->kind) {
        case 0:
        case 1:
        default:
            break;
        case 2:
        case 3:
            ContactCollider_SetReactionFlags(&p->colliderGroup.colliders[2], 0x20);
            addCategory(&p->colliderGroup.colliders[2], 0x10);
            ContactCollider_SetReactionFlags(&p->colliderGroup.colliders[0], 0x20);
            ContactCollider_SetReactionFlags(&p->colliderGroup.colliders[1], 0x20);
            break;
        case 4:
        case 5:
            ContactCollider_SetReactionFlags(&p->colliderGroup.colliders[2], 0x10);
            addCategory(&p->colliderGroup.colliders[2], 0x20);
            ContactCollider_SetReactionFlags(&p->colliderGroup.colliders[0], 0x10);
            ContactCollider_SetReactionFlags(&p->colliderGroup.colliders[1], 0x10);
            break;
        case 6:
        case 7:
        case 12:
        case 13:
            ContactCollider_SetReactionFlags(&p->colliderGroup.colliders[2], 4);
            addCategory(&p->colliderGroup.colliders[2], 8);
            ContactCollider_SetReactionFlags(&p->colliderGroup.colliders[0], 4);
            ContactCollider_SetReactionFlags(&p->colliderGroup.colliders[1], 4);
            break;
        case 8:
        case 9:
        case 14:
        case 15:
            ContactCollider_SetReactionFlags(&p->colliderGroup.colliders[2], 8);
            addCategory(&p->colliderGroup.colliders[2], 4);
            ContactCollider_SetReactionFlags(&p->colliderGroup.colliders[0], 8);
            ContactCollider_SetReactionFlags(&p->colliderGroup.colliders[1], 8);
            break;
        case 10:
            ContactCollider_SetReactionFlags(&p->colliderGroup.colliders[2], 1);
            addCategory(&p->colliderGroup.colliders[2], 2);
            ContactCollider_SetReactionFlags(&p->colliderGroup.colliders[0], 1);
            ContactCollider_SetReactionFlags(&p->colliderGroup.colliders[1], 1);
            break;
        case 11:
            ContactCollider_SetReactionFlags(&p->colliderGroup.colliders[2], 2);
            addCategory(&p->colliderGroup.colliders[2], 1);
            ContactCollider_SetReactionFlags(&p->colliderGroup.colliders[0], 2);
            ContactCollider_SetReactionFlags(&p->colliderGroup.colliders[1], 2);
            break;
    }
}
