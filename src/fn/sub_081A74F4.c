#define Vanargand_UpdateMode2 sub_081A74F4
#include "global.h"
/* Tentative boss state names; offsets agree with the timed mode sibling. */
struct VanargandBoss {
    u8 unk0[0xaa];
    u8 actionMode;
    u8 unkab[3];
    u8 nextMode;
    u8 actionComplete;
    u8 modeEntry;
    u8 unkb1[0x13];
    u32 modeTimer;
    u8 unkc8[0x50];
    u8 trigger[0x10];
    u8 unk128[0xf4];
    u8 actionSequence[0x18];
    u8 unk234[0x338];
    void *pair;
};
void Vanargand_StopActionSequence(void *);
s32 ScenePair_CountPositiveCounters(void *);
u32 Trigger_ConsumePending(void *, u32);
static inline u8 Vanargand_ConsumeModeEntry(struct VanargandBoss *boss)
{
    u8 *entry = &boss->modeEntry;
    if (*entry != 0) {
        *entry = 0;
        *(entry - 1) = 0;
        return TRUE;
    }
    return FALSE;
}
void sub_081A74F4(struct VanargandBoss *boss)
{
    struct VanargandBoss *owner = boss;
    if (Vanargand_ConsumeModeEntry(boss) == 1) {
        u32 mode = 2;
        Vanargand_StopActionSequence(boss->actionSequence);
        boss->actionMode = mode;
    }
    if (boss->actionComplete) {
        if (ScenePair_CountPositiveCounters(owner->pair) <= 1) {
            u32 next = 3;
            boss->nextMode = next;
            boss->modeTimer = 0;
            boss->modeEntry = 1;
        } else {
            Trigger_ConsumePending(boss->trigger, 1);
        }
    }
}
