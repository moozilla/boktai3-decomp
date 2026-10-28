#include "global.h"
#include "engine.h"
#include "player.h"
struct ItemDef {
    u16 unknown0;
    u16 actionKind;
    u16 unknown4;
    u16 valueA;
    u16 valueB;
    u8 unknownA[6];
};
extern const struct ItemDef gUnk_08E61280[];
struct ItemUser {
    u8 pad0[0x30];
    u8 position[0x230 - 0x30];
    u8 collider[0x424 - 0x230];
    u16 health, maxHealth;
    u8 pad428[0x534 - 0x428];
    u16 effectValue, effectItem, effectTimer;
    u8 pad53A[0xAD6 - 0x53A];
    u16 cooldown, cooldownFrames;
    u8 padADA[0xAE8 - 0xADA];
    u8 numberOffset[8];
};
s32 Script_GetActorIndex(void);
s32 Div(s32, s32);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
// confidence=low: script command that applies item 'E' to the actor: kind 5 starts a timed
// effect (duration from 't' or valueB seconds), kind 1 heals valueA percent of max HP and shows
// the amount.
// MATCHING_CONCESSION: register-pinning
#define ITEM_USER_OFFSET(field) ((u32) & ((struct ItemUser *)0)->field)
void sub_0815A73C(void)
{
    struct ItemUser *p;
    s32 item;
    s32 value;
    const struct ItemDef *def;
    const struct ItemDef *table;
    s32 i = Script_GetActorIndex();
    p = gPlayers[i];
    if ((p == ((void *)0)) || (!Script_SeekToKeyword(0x45)))
        return;
    item = Script_GetValue();
    table = gUnk_08E61280;
    def = table + item;
    if (def->actionKind == 5) {
        if (Script_SeekToKeyword(0x74))
            value = Script_GetValue();
        else
            value = def->valueB * 60;
        p->effectValue = gUnk_08E61280[item].valueA;
        p->effectTimer = value;
        p->effectItem = item;
    } else if (def->actionKind == 1) {
        register u32 maxHealthOffset asm("r1") = ITEM_USER_OFFSET(maxHealth);
        u16 *maxHealth;
        u16 *health;
        register u32 healthOffset asm("r2");
        s32 amount;
        // Keep the field offset in its assigned register and the actor live here.
        asm("" : "+r"(maxHealthOffset) : "r"(p));
        maxHealth = (u16 *)((u8 *)p + maxHealthOffset);
        amount = Div((*maxHealth) * def->valueA, 100);
        healthOffset = ITEM_USER_OFFSET(health);
        // Preserve the field offset register across address formation.
        asm("" : "+r"(healthOffset));
        health = (u16 *)((u8 *)p + healthOffset);
        if (((*health) + amount) > (*maxHealth)) {
            amount = (*maxHealth) - (*health);
            *health = *maxHealth;
        } else
            *health = (*health) + amount;
        if (amount > 0)
            DamageNumberDisplay_Show(p->collider, amount, 1, p->position, p->numberOffset, 0);
    } else
        return;
    p->cooldown = 0x16E;
    p->cooldownFrames = 0x30;
}
