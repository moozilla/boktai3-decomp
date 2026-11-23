#include "global.h"
#include "ram.h"
#include "word_flags.h"

struct GrowthHazardAnimationEvent {
    u8 unk0[4];
    u16 identifier;
    u8 unk6[4];
    u16 flagsA;
    u8 unkC[0x2C];
    u32 flags38;
    u8 unk3C[8];
    u16 cooldown;
};
struct GrowthHazardEntry {
    u8 unk0[0xF6];
    u16 mode;
    u16 eventCount;
    u8 unkFA[2];
    s16 remaining;
    u8 unkFE[2];
    u8 cooldown;
    u8 unk101;
    u8 state;
};
struct GrowthHazardManager {
    u8 unk0[0x19];
    u8 flags;
};
u32 Hresvelgr_GetAnimationIdentifier(void);

void GrowthHazard_ApplyAnimationEvent(struct GrowthHazardAnimationEvent *event, u32 unused,
                                      struct GrowthHazardEntry *entry)
{
    u32 identifier = event->identifier;
    if (identifier != Hresvelgr_GetAnimationIdentifier()) {
        switch (entry->state) {
            case 0:
                if (entry->cooldown == 0 && WordFlags_Test(&event->flags38, 0x20)) {
                    entry->eventCount++;
                    entry->cooldown = event->cooldown;
                    entry->mode = 0;
                    ((struct GrowthHazardManager *)gGrowthHazardManager)->flags |= 1;
                }
                break;
            case 2:
                if (HalfwordFlags_Test(0x10, &event->flagsA) && entry->cooldown == 0 &&
                    entry->remaining > 0) {
                    if (--entry->remaining > 0) {
                        entry->cooldown = 0x20;
                        entry->mode = 1;
                        ((struct GrowthHazardManager *)gGrowthHazardManager)->flags |= 2;
                    }
                }
                break;
        }
    }
}
