#include "global.h"
struct SceneCombatCollider {
    u8 unknown0[0x34];
    u32 categories, flags;
    u16 power;
    u8 unknown3e[0x16];
};
struct SceneCombatState {
    u8 phase, mode;
    u16 counter, timer, value6, value8, valueA, savedAttack, savedDefense;
};
struct SceneCombatActor {
    u8 unknown0[0x190];
    struct SceneCombatCollider colliders[3];
    u8 unknown28c[0xb26 - 0x28c];
    u16 attack, defense;
    u8 unknownb2a[8];
    u16 flags;
    u8 unknownb34[0x1120 - 0xb34];
    struct SceneCombatState state;
};
static inline void SetDefenseCategories(struct SceneCombatCollider *c)
{
    u32 mask = ~8;
    u32 flags = c->flags & mask;
    u32 mask2 = ~4;
    u32 categories = c->categories & mask2;
    c->flags = flags | 4;
    c->categories = categories | 8;
}
void SceneCombatActor_ResetDefenseMode(struct SceneCombatActor *p)
{
    struct SceneCombatState saved = p->state;
    struct SceneCombatCollider *a, *b, *c;
    p->state.savedAttack = saved.savedAttack;
    p->state.savedDefense = saved.savedDefense;
    p->state.phase = 0;
    p->state.value8 = 0;
    p->state.valueA = 0;
    p->state.counter = 0;
    p->state.timer = 0;
    p->state.value6 = 0;
    p->attack = p->state.savedAttack;
    p->defense = p->state.savedDefense;
    p->colliders[2].power = p->defense;
    a = &p->colliders[1];
    {
        u32 mask = ~0x400008;
        a->flags &= mask;
        b = &p->colliders[0];
        b->flags &= mask;
    }
    c = &p->colliders[2];
    SetDefenseCategories(c);
    b->flags |= 4;
    a->flags |= 4;
    p->flags |= 0x200;
}
