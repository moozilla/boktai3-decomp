// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
struct TetherExternalStatus {
    u8 unknown0[0x5AA];
    s16 value;
};

extern const u16 gUnk_086059F0[];
s32 sub_0804A418(void);
void TetherActor_SetState(struct TetherActor *, u32);
void TetherActor_InitKindFromScript(struct TetherActor *actor, u32 entry)
{
    register s32 kind asm("r0");
    s32 value;
    actor->entry = entry;
    if (Script_SeekToKeyword('k'))
        kind = Script_GetValue();
    else
        kind = sub_0804A418();
    ((struct TetherExternalStatus *)gUnk_02000710)->value = kind;
    asm("" ::: "memory");
    actor->kind = ((struct TetherExternalStatus *)gUnk_02000710)->value;
    value = Script_SeekToKeyword('c');
    if (value)
        value = Script_GetValue();
    actor->kindValue = value;
    actor->kindLimit = gUnk_086059F0[actor->kind];
    if (Script_SeekToKeyword('a'))
        TetherActor_SetState(actor, Script_GetValue());
    else
        TetherActor_SetState(actor, 0);
    if (Script_SeekToKeyword('A')) {
        actor->valueA = Script_GetValue();
        actor->valueB = Script_GetValue();
    } else {
        actor->valueA = 50;
        actor->valueB = 50;
    }
}
