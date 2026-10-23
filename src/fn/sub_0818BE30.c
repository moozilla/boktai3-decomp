#include "global.h"
struct ArmorCollider {
    u8 pad0[6];
    u16 flags;
    u8 pad8[0x34 - 8];
    u32 category, properties;
    u8 pad3C[0x54 - 0x3C];
};
struct ArmorColliderOwner {
    u8 pad0[0xD4];
    u8 position[8];
    u8 padDC[0x190 - 0xDC];
    struct ArmorCollider attack, secondaryAttack, contact, trigger;
    u8 pad2E0[0xB26 - 0x2E0];
    s16 attackPower, defensePower;
    u8 padB2A[0xB38 - 0xB2A];
    u8 type;
};
extern const u8 gUnk_0824F10C[], gUnk_0824F114[], gUnk_0824F11C[];
void Armor_InitCollider(void *, u32, void *, const void *, const void *, void *, void *);
void Armor_InitExtendedCollider(void *, u32, s32, void *, const void *, const void *, void *,
                                void *);
void Collider_SetAttackProperties(void *, s32, s32, u32, u32, s32);
void Armor_ReceiveColliderContact(void);
void Armor_TriggerBehavior35(void);
static inline void addFlags(struct ArmorCollider *p, u32 mask)
{
    p->flags |= mask;
}
static inline void addProperties(struct ArmorCollider *p, u32 mask)
{
    p->properties |= mask;
}
static inline void addCategory(struct ArmorCollider *p, u32 mask)
{
    p->category |= mask;
}
void Armor_InitCombatColliders(struct ArmorColliderOwner *input, u32 inputId)
{
    struct ArmorCollider *c_2;
    struct ArmorColliderOwner *p = input;
    u32 id = inputId;
    s32 value, power;
    s32 defense;
    struct ArmorCollider *c;
    void *position;

    Armor_InitCollider(&p->secondaryAttack, id, p->position, gUnk_0824F10C, gUnk_0824F114, 0, 0);
    value = p->attackPower << 10;
    if (value >= 0) {
        power = value >> 12;
    } else {
        power = -(-value >> 12);
    }
    c = &p->secondaryAttack;
    Collider_SetAttackProperties(c, power, 50, 0, 0x40000, 30);
    c->flags |= 4;
    c_2 = &p->contact;
    defense = p->defensePower;
    position = p->position;
    Armor_InitExtendedCollider(c_2, id, defense, position, gUnk_0824F10C, gUnk_0824F114,
                               Armor_ReceiveColliderContact, p);
    addFlags(c_2, 0x100C);
    c = &p->trigger;
    Armor_InitExtendedCollider(c, id, 0, position, gUnk_0824F10C, gUnk_0824F11C,
                               Armor_TriggerBehavior35, p);
    c->flags |= 6;
    Armor_InitCollider(&p->attack, id, position, gUnk_0824F10C, gUnk_0824F114, 0, p);
    value = p->attackPower << 11;
    if (value >= 0) {
        power = value >> 12;
    } else {
        power = -(-value >> 12);
    }
    c = &p->attack;
    Collider_SetAttackProperties(c, power, 50, 0x2000, 0x60000, 30);
    c->flags |= 4;
    switch (p->type) {
        case 0:
        case 1:
        default:
            break;
        case 2:
        case 3:
            addProperties(&p->contact, 0x20);
            addCategory(&p->contact, 0x10);
            addProperties(&p->attack, 0x20);
            addProperties(&p->secondaryAttack, 0x20);
            break;
        case 4:
        case 5:
            addProperties(&p->contact, 0x10);
            addCategory(&p->contact, 0x20);
            addProperties(&p->attack, 0x10);
            addProperties(&p->secondaryAttack, 0x10);
            break;
        case 6:
        case 7:
        case 12:
        case 13:
            addProperties(&p->contact, 4);
            addCategory(&p->contact, 8);
            addProperties(&p->attack, 4);
            addProperties(&p->secondaryAttack, 4);
            break;
        case 8:
        case 9:
        case 14:
        case 15:
            addProperties(&p->contact, 8);
            addCategory(&p->contact, 4);
            addProperties(&p->attack, 8);
            addProperties(&p->secondaryAttack, 8);
            break;
        case 10:
            addProperties(&p->contact, 1);
            addCategory(&p->contact, 2);
            addProperties(&p->attack, 1);
            addProperties(&p->secondaryAttack, 1);
            break;
        case 11:
            addProperties(&p->contact, 2);
            addCategory(&p->contact, 1);
            addProperties(&p->attack, 2);
            addProperties(&p->secondaryAttack, 2);
            break;
    }
}
