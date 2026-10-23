#include "global.h"

#define Vanargand_OnDamage sub_081ADBF8
struct VanargandDamage {
    u8 pad0[0x90];
    u32 timer;
    s16 health;
    u8 pad96[0xA2 - 0x96];
    u8 phase, frame;
    u32 phaseTimer;
    u8 padA8[3];
    u8 requestedState, actionActive, actionReset, action;
    u8 padAF;
    u8 actionPending, pendingAnimation;
    u8 padB2[0xC4 - 0xB2];
    u32 actionTimer;
    u8 padC8[0x100 - 0xC8];
    void (*callback)(void *);
};
void ActorSoundPhase_StartTimedAnimation(void *);
void Vanargand_UpdateOffsetMotion(void *);
void Vanargand_UpdatePaletteLeap(void *);
void DamageNumberDisplay_ShowWithDefaultScale(void *, s32, s32, void *, s32);
static inline void StartAction(struct VanargandDamage *p, s32 one, s32 zero, s32 id)
{
    p->actionActive = one;
    p->actionReset = zero;
    p->action = id;
    p->actionTimer = zero;
    p->actionPending = one;
}
static inline void SetState(struct VanargandDamage *p, void (*callback)(void *), s32 id)
{
    s32 zero;
    p->requestedState = id;
    zero = 0;
    p->callback = callback;
    p->timer = zero;
    p->pendingAnimation = 1;
    p->phase = zero;
    p->phaseTimer = zero;
}
s32 Vanargand_OnDamage(struct VanargandDamage *p, s32 kind, s32 damage)
{
    switch (kind) {
        case 0:
            if (damage > 0)
                DamageNumberDisplay_ShowWithDefaultScale((u8 *)p + 0x6C4, damage, 0,
                                                         (u8 *)p + 0x1AC, 1);
            StartAction(p, 1, 0, 4);
            SetState(p, ActorSoundPhase_StartTimedAnimation, 24);
            break;
        case 2:
            StartAction(p, 1, 0, 5);
            SetState(p, Vanargand_UpdateOffsetMotion, 25);
            break;
        case 1:
            p->health = 0;
            StartAction(p, 1, 0, 6);
            SetState(p, Vanargand_UpdatePaletteLeap, 20);
            break;
    }
    return 1;
}
