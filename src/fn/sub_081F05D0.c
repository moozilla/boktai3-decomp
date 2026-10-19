
#ifdef NONMATCHING
// fuzzy: 97.1%
// foundry draft (worker best 97.1%); what differs is mostly register allocation and instruction order
#define SingletonSlot7_OnTargetContact sub_081F05D0
#include "global.h"
#include "game_state.h"
struct Slot7Actor {
    u8 pad0[0x5c0];
    u16 duration, flashTimer;
    u8 pad5c4[0x2e];
    s16 contactDuration;
};
struct Slot7Target { u8 pad0[0x50]; struct Slot7Actor *owner; };
void Sound_PlaySong(u32);
s32 sub_081F05D0(struct Slot7Target *target, s32 event, s32 amount)
{
    struct Slot7Actor *actor = target->owner;
    u32 duration;
    s32 level;
    if (event == 0) {
        actor->flashTimer = 15;
        if (amount > 0) {
            level = gUnk_02000710->unk876 + 1;
            if (level <= 2)
                duration = (u16)actor->contactDuration;
            else if (level <= 5)
                {
                u16 base = actor->contactDuration;
                duration = base + ((s16)base >> 2);
            }
            else
                duration = (u16)actor->contactDuration + (actor->contactDuration >> 1);
            actor->duration = duration;
        }
        if (amount > 0)
            Sound_PlaySong(0x5c9);
    } else if (event != 1)
        return 0;
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081F05D0);
#endif
