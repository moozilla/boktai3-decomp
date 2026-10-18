#include "global.h"
struct SceneCombatCollider {
    u8 unknown0[0x34];
    u32 categories, flags;
    u16 power;
    u8 unknown3e[0x16];
};
struct SceneCombatState {
    u8 phase, mode;
    u16 counter, timer, value6, value8, valueA, savedAttack, savedDefense, value10, value12;
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
static inline void SetAttackCategories(struct SceneCombatCollider *c)
{
    u32 mask = ~4;
    u32 flags = c->flags & mask;
    u32 mask2 = ~8;
    u32 categories = c->categories & mask2;
    c->flags = flags | 8;
    c->categories = categories | 4;
}
void SceneCombatActor_ResetAttackMode(struct SceneCombatActor *p)
{
    struct SceneCombatState saved = p->state;
    struct SceneCombatCollider *a, *b, *c;
    p->state.savedAttack = saved.savedAttack;
    p->state.savedDefense = saved.savedDefense;
    p->state.phase = 0;
    p->state.value6 = 0;
    p->state.value8 = 0;
    p->state.mode = 0;
    p->state.counter = 0;
    p->state.timer = 420;
    p->state.valueA = 0;
    p->attack = p->state.savedAttack;
    p->defense = p->state.savedDefense;
    p->colliders[2].power = p->defense;
    a = &p->colliders[1];
    {
        u32 mask = ~0x200004;
        a->flags &= mask;
        b = &p->colliders[0];
        b->flags &= mask;
    }
    c = &p->colliders[2];
    SetAttackCategories(c);
    b->flags |= 8;
    a->flags |= 8;
    p->flags |= 0x200;
}
