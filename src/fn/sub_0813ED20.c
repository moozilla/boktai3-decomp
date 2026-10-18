#define Player_ConsumeActionCharge   sub_0813ED20
#define Player_BeginDescriptorAction sub_0813EABC
#include "global.h"
#include "player.h"
void sub_08178370(void);
void sub_0813EABC(struct Player *, struct PlayerActionEntry *);
static inline u32 TestFlags(u32 *flags, u32 mask)
{
    u32 m = mask;
    return *flags & m;
}
static inline s32 HasFlag(struct Player *p, u32 mask)
{
    if (!p)
        return FALSE;
    if (TestFlags(&p->statBonusFlags, mask))
        return TRUE;
    return FALSE;
}
void sub_0813ED20(struct Player *p)
{
    s32 cost;
    struct PlayerActionEntry *entry;

    if (p->actionMode != 6) {
        {
            struct PlayerActionDrainContext *drain = &p->statModifierContext.drain;
            cost = drain->pendingCost;
        }
        if (cost > 0) {
            if (!p->statModifierContext.statBOverrideEnabled) {
                s32 kind = p->statModifierContext.actionEntry->kind;
                if (kind == 0x30)
                    goto clear;
                if (kind == 0x2F)
                    goto clear;
                {
                    if (HasFlag(p, 0x400))
                        cost <<= 1;
                    if (HasFlag(p, 1) || HasFlag(p, 0x200))
                        cost >>= 1;
                    if (cost <= 0)
                        cost = 1;
                    entry = p->statModifierContext.actionEntry;
                    if (entry->remainingCharge != 0) {
                        if (entry->remainingCharge <= cost) {
                            entry->remainingCharge = 0;
                            sub_08178370();
                            sub_0813EABC(p, p->statModifierContext.actionEntry);
                        } else
                            entry->remainingCharge -= cost;
                    }
                }
            }
        clear: {
            struct PlayerActionDrainContext *drain = &p->statModifierContext.drain;
            drain->pendingCost = 0;
        }
        }
    }
}
