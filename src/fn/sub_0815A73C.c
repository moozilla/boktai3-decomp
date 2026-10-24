#include "global.h"
#include "engine.h"
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
extern struct ItemUser *gUnk_02000580[];
s32 Script_GetActorIndex(void);
s32 Div(s32, s32);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
// confidence=low: script command that applies item 'E' to the actor: kind 5 starts a timed
// effect (duration from 't' or valueB seconds), kind 1 heals valueA percent of max HP and shows
// the amount.
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Only register allocation differs: the ROM keeps the actor in r5 and the item definition in r6.
void sub_0815A73C(void)
{
    struct ItemUser *p;
    s32 item;
    s32 value;
    const struct ItemDef *def;
    s32 i = Script_GetActorIndex();
    p = gUnk_02000580[i];
    if ((p == ((void *)0)) || (!Script_SeekToKeyword(0x45)))
        return;
    item = Script_GetValue();
    if (def->actionKind == 5) {
        if (Script_SeekToKeyword(0x74))
            value = Script_GetValue();
        else
            value = def->valueB * 60;
        p->effectValue = gUnk_08E61280[item].valueA;
        p->effectTimer = value;
        p->effectItem = item;
    } else if (def->actionKind == 1) {
        u16 *maxHealth = &p->maxHealth;
        u16 *health;
        s32 amount = Div((*maxHealth) * def->valueA, 100);
        health = &p->health;
        if (((*health) + amount) > (*maxHealth)) {
            amount = (*maxHealth) - (*health);
            *health = *maxHealth;
        } else
            *health = (*health) + amount;
        if (amount > 0)
            DamageNumberDisplay_Show(p->collider, amount, 1, p->position, p->numberOffset, 0);
    } else
        return;
    def = &gUnk_08E61280[item];
    p->cooldown = 0x16E;
    p->cooldownFrames = 0x30;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0815A73C);
#endif
