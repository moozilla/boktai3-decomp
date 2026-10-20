#include "global.h"
struct ArmorContactCollider {
    u8 pad0[6];
    u16 flags;
    u16 pad8, contactId;
    s32 position[3];
    u8 pad18[0x34 - 0x18];
    u32 category, properties;
    u16 pad3C, damage;
    u8 pad40[2];
    u8 direction, effect;
    u16 cooldown;
};
struct ArmorContactOwner {
    u8 pad0[0xB18];
    u32 actorId;
    s16 health, maxHealth, guardHealth;
    u8 padB22[2];
    s16 guardThreshold;
    u8 padB26[6];
    s16 contactCooldown;
    u16 lastContactId;
    s16 contactHundredths;
    u16 flags;
    u8 padB34[4];
    u8 type, direction;
    u8 padB3A[0xB50 - 0xB3A];
    u32 contactScript;
    u8 padB54[0x1120 - 0xB54];
    u8 state;
};
void Collision_CalculateDamageResponse(struct ArmorContactCollider *,
                                       struct ArmorContactCollider *);
void PlayerStats_AddContactHundredths(void *, void *, s32);
void ActorController_SetBehavior(void *, s32);
void ActorPalette_Reset(void *);
void ActorMarker_EmitRingEffects(void *);
void Armor_SetPlanarVelocity(void *, s32, s32);
void Armor_UpdateBehaviorAfterContact(void *, s32, u32, s32);
void SceneHud_SetTimedState(u32, s32, s32, s32, s32, s32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void Sound_PlaySongChecked(u32);
struct ContactScriptArgs {
    u32 count : 16;
    u32 pad : 16;
    s32 *values;
};
void Script_Call(u32, void *);

static inline u32 properties(struct ArmorContactCollider *p, u32 mask)
{
    return p->properties & mask;
}

void Armor_ReceiveColliderContact(struct ArmorContactCollider *attack,
                                  struct ArmorContactCollider *contact,
                                  struct ArmorContactOwner *input)
{
    struct ContactScriptArgs args;
    struct ArmorContactOwner *p;
    s32 originalDamage;
    s32 damage;
    s32 knockback;
    s32 critical;
    s32 direction;
    s32 broken;
    s32 sound;
    u32 savedProperties, savedCategory;
    broken = 0;
    p = input;
    if (p->flags & 0x80)
        return;
    if (p->contactCooldown > 0)
        return;
    p->contactCooldown = attack->cooldown;
    Collision_CalculateDamageResponse(attack, contact);
    PlayerStats_AddContactHundredths(attack, contact, p->contactHundredths);
    critical = 0;
    damage = originalDamage = contact->damage;
    knockback = 30;
    if (attack->flags & 0x100) {
        direction = attack->direction;
        {
            s32 facing = direction + 0xA0;
            p->direction = facing & -64;
        }
    } else {
        direction = (p->direction + 0xA0) & 0xC0;
    }
    if (p->flags & 2) {
        if (contact->effect & 8) {
            ActorController_SetBehavior(p, 16);
            ActorPalette_Reset(p);
            if (contact->effect & 4)
                ActorMarker_EmitRingEffects(p);
            sound = 1;
        } else {
            p->guardHealth -= damage;
            if (p->guardHealth <= 0) {
                ActorController_SetBehavior(p, 14);
                ActorPalette_Reset(p);
                broken = 1;
                sound = 3;
            } else {
                ActorController_SetBehavior(p, 13);
                ActorPalette_Reset(p);
                sound = 2;
            }
            if (!broken) {
                savedProperties = contact->properties;
                savedCategory = contact->category;
                switch (p->type) {
                    case 3:
                    case 4:
                        contact->properties = 0x1F;
                        contact->category = 0x20;
                        Collision_CalculateDamageResponse(attack, contact);
                        if (properties(attack, 0x10))
                            damage = -damage;
                        else if (properties(attack, 0x20))
                            ActorMarker_EmitRingEffects(p);
                        else {
                            knockback >>= 2;
                            damage >>= 1;
                        }
                        break;
                    case 5:
                        contact->properties = 0x2F;
                        contact->category = 0x10;
                        Collision_CalculateDamageResponse(attack, contact);
                        contact->properties = savedProperties;
                        contact->category = savedCategory;
                        if (properties(attack, 0x20))
                            damage = -damage;
                        else if (properties(attack, 0x10))
                            ActorMarker_EmitRingEffects(p);
                        else {
                            knockback >>= 2;
                            damage >>= 1;
                        }
                        break;
                    case 7:
                    case 13:
                        contact->properties = 0x3B;
                        contact->category = 4;
                        Collision_CalculateDamageResponse(attack, contact);
                        if (properties(attack, 8))
                            damage = -damage;
                        else if (properties(attack, 4))
                            ActorMarker_EmitRingEffects(p);
                        else {
                            knockback >>= 2;
                            damage >>= 1;
                        }
                        break;
                    case 9:
                    case 15:
                        contact->properties = 0x37;
                        contact->category = 8;
                        Collision_CalculateDamageResponse(attack, contact);
                        if (properties(attack, 4))
                            damage = -damage;
                        else if (properties(attack, 8))
                            ActorMarker_EmitRingEffects(p);
                        else {
                            knockback >>= 2;
                            damage >>= 1;
                        }
                        break;
                    case 10:
                        contact->properties = 0x3E;
                        contact->category = 1;
                        Collision_CalculateDamageResponse(attack, contact);
                        if (properties(attack, 2))
                            damage = -damage;
                        else if (properties(attack, 1))
                            ActorMarker_EmitRingEffects(p);
                        else {
                            knockback >>= 2;
                            damage >>= 1;
                        }
                        break;
                    case 11:
                        contact->properties = 0x3D;
                        contact->category = 2;
                        Collision_CalculateDamageResponse(attack, contact);
                        if (properties(attack, 1))
                            damage = -damage;
                        else if (!(properties(attack, 2))) {
                            knockback >>= 2;
                            damage >>= 1;
                        } else
                            ActorMarker_EmitRingEffects(p);
                        break;
                    default:
                        knockback >>= 2;
                        damage >>= 1;
                        break;
                }
                contact->properties = savedProperties;
                contact->category = savedCategory;
            }
        }
    } else {
        if (contact->effect & 8) {
            critical = 1;
            sound = 1;
        } else {
            knockback >>= 1;
            sound = 0;
        }
        if (contact->effect & 4)
            ActorMarker_EmitRingEffects(p);
        if ((u8)(p->type - 2) <= 1 && p->state == 16) {
            knockback = 0;
        } else {
            if (critical)
                ActorController_SetBehavior(p, 16);
            else
                ActorController_SetBehavior(p, 15);
            ActorPalette_Reset(p);
        }
    }
    if (!(properties(attack, 0x40))) {
        if (damage <= 0)
            damage = 1;
    }
    if (damage == 0)
        sound = 3;
    p->health -= damage;
    if (knockback > 0)
        Armor_SetPlanarVelocity(p, direction, knockback);
    if (p->health <= 0) {
        ActorController_SetBehavior(p, 18);
        p->flags |= 0x80;
        p->health = 0;
    } else {
        if (p->health > p->maxHealth)
            p->health = p->maxHealth;
        if ((p->flags & 1) && p->health < p->guardThreshold) {
            ActorController_SetBehavior(p, 14);
            broken = 1;
            sound = 3;
        }
        Armor_UpdateBehaviorAfterContact(p, originalDamage, attack->properties, broken);
    }
    SceneHud_SetTimedState(p->actorId, p->health, p->maxHealth, 5, critical, -1);
    if (damage < 0) {
        DamageNumberDisplay_Show(contact, -damage, 1, &contact->position[0], &contact->position[2],
                                 1);
    } else {
        DamageNumberDisplay_Show(contact, damage, 0, &contact->position[0], &contact->position[2],
                                 1);
        p->lastContactId = attack->contactId;
    }
    switch (sound) {
        case 0:
            Sound_PlaySongChecked(0x13E);
            break;
        case 1:
            Sound_PlaySongChecked(0x13E);
            Sound_PlaySongChecked(0x16E);
            break;
        case 2:
            Sound_PlaySongChecked(0xBC);
            break;
        case 3:
            Sound_PlaySongChecked(0xBD);
            break;
    }
    if ((p->flags & 0x100) && p->contactScript) {
        args.count = 0;
        Script_Call(p->contactScript, &args);
    }
    if (broken)
        p->flags |= 0x100;
    else
        p->flags &= ~0x100;
}
