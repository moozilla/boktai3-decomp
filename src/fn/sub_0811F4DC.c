#define ModifierAction_ActivateCost sub_0811F4DC
#define ModifierAction_CanActivate Modifier_IsFourthSourceReady
#define ModifierAction_ConsumeCost sub_0811F490
#include "global.h"
struct ModifierActionOwner {
    u8 unknown0[0x18];
    u8 stats[0x58];
    s16 available;
};
struct ModifierAction {
    u8 unknown0[6];
    u16 duration, cost;
    u8 unknownA[2];
    u16 interval;
    u8 unknownE[3];
    u8 status;
    u16 remaining, cooldown;
    u8 unknown16[2];
    struct ModifierActionOwner *owner;
};
u8 Modifier_IsFourthSourceReady(struct ModifierAction *);
s32 sub_0811EAF8(void *, s32, s32);
void sub_0811F490(void *, struct ModifierAction *, s32);
void Sound_PlaySong(u32);
s32 sub_0811F4DC(struct ModifierAction *action)
{
    struct ModifierActionOwner *owner;
    void *stats;
    s32 cost;
    action->status = 0xff;
    if (Modifier_IsFourthSourceReady(action)) {
        owner = action->owner;
        stats = owner->stats;
        if (!action->remaining) {
            cost = sub_0811EAF8(stats, action->cost, 10);
            if (owner->available >= cost) {
                sub_0811F490(stats, action, cost);
                action->remaining = action->duration;
                action->cooldown = action->interval;
                Sound_PlaySong(0x21e);
            }
        }
    }
    return 0;
}
