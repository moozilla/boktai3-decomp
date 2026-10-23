#include "global.h"
struct ScreenSelectionPhase {
    u8 unknown0[0xAF];
    u8 pending;
    u8 entered;
    u8 unknownB1[0x13];
    u32 timer;
    u8 unknownC8[0x1B0];
    u16 selection;
    u8 unknown27A[0x1A];
    u32 requestedSelection;
    u8 unknown298[0x324];
    u32 active;
};
u32 Trigger_ConsumePending(void *, u32);
static inline u8 TakeEntry(struct ScreenSelectionPhase *screen)
{
    if (screen->entered) {
        screen->entered = 0;
        screen->pending = 0;
        return 1;
    }
    return 0;
}
// confidence=low: starts a screen-selection transition from coordinate minus one hundred.
#define SingletonSlot7_BeginOffscreenSelection sub_081EF750
void SingletonSlot7_BeginOffscreenSelection(struct ScreenSelectionPhase *screen)
{
    struct ScreenSelectionPhase *state = screen;
    if (TakeEntry(screen)) {
        state->selection = -100;
        state->requestedSelection = -100;
        Trigger_ConsumePending((u8 *)screen + 0x118, 1);
    }
    screen->timer++;
}
