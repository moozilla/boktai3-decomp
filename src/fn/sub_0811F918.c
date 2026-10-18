#define ModifierAction_SpawnBurst sub_0811F918
#define ModifierAction_CanActivate Modifier_IsFourthSourceReady
#define ModifierAction_ConsumeCost sub_0811F490
#include "global.h"
#include "modifier_action.h"
struct ModifierActionActor {
    u8 unknown0[0x7e];
    u16 parameter;
    u8 unknown80[0x294];
    struct ModifierAction action;
};
u8 Modifier_IsFourthSourceReady(struct ModifierAction *);
void sub_0811F490(void *, struct ModifierAction *, s32);
void sub_0811D6C8(void *, s32, s32, void *, s32, s32, s32, s32);
void Sound_PlaySong(u32);
s32 sub_0811F918(struct ModifierActionActor *actor, void *target)
{
    struct ModifierAction *action = &actor->action;
    action->status = 0xff;
    if (Modifier_IsFourthSourceReady(action)) {
        u32 zero = action->remaining;
        if (!zero) {
            action->remaining = action->duration;
            sub_0811F490(actor, action, 0);
            sub_0811D6C8(actor, 0x385, 0x385, target, actor->parameter, 16, 4, zero);
            action->cooldown = action->interval;
            Sound_PlaySong(0x3e3);
        }
    }
    return 0;
}
