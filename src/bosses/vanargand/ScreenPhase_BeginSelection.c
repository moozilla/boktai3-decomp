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
void ScreenPhase_BeginSelection(struct ScreenSelectionPhase *screen)
{
    struct ScreenSelectionPhase *state = screen;
    if (TakeEntry(screen)) {
        state->selection = state->requestedSelection;
        Trigger_ConsumePending((u8 *)screen + 0x118, 1);
    }
    state->active = 1;
    screen->timer++;
}
