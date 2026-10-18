#include "global.h"
struct P {
    void (*cb)(void);
    u8 p0;
    u8 sel;
    u8 pad[0xa];
    u8 fl;
    u8 pad2[7];
    u32 v;
};
void Memory_Clear(void *, u32);
s32 ScriptRecord_FindAttributeModifier(void *, s32);
void CallbackStatus_ResetSelectorAndTest(void);
void ActorAbility_SpawnProjectile(void);
void ActorAbility_ActivatePaletteEffect(void);
void ActorAbility_SpawnAlternateProjectile(void);
void sub_0811F7EC(void);
void ModifierAction_ActivateCost(void);
void ActorAbility_UpdateRecovery(void);
void Modifier_InitFourthSourceEffect(u8 *s, u32 v)
{
    u8 *a = s + 0x48;
    struct P *p = (struct P *)(s + 0x314);
    u32 z = 0;
    void (*f)(void);
    u8 *q;
    p->cb = (void (*)(void))z;
    p->fl = z;
    p->v = v;
    q = s + 0x318;
    Memory_Clear(q, 0xc);
    if (ScriptRecord_FindAttributeModifier(q, *(s8 *)(a + 0xe)) < 0) {
        p->cb = CallbackStatus_ResetSelectorAndTest;
        p->fl = z;
    } else {
        switch (p->sel) {
            case 0:
                f = ActorAbility_SpawnProjectile;
                goto set;
            case 1:
                f = ActorAbility_ActivatePaletteEffect;
                goto set;
            case 2:
                f = ActorAbility_SpawnAlternateProjectile;
                goto set;
            case 3:
                f = sub_0811F7EC;
                goto set;
            case 4:
                f = ModifierAction_ActivateCost;
                goto set;
            case 5:
                f = ActorAbility_UpdateRecovery;
            set:
                p->cb = f;
                p->fl = 1;
                break;
        }
    }
}
