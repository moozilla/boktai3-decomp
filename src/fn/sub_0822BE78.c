#include "global.h"
struct State {
    u8 pad[8];
    u8 slot;
};
extern struct State *gSessionState;
void Save_SnapshotContinuation(void), GameState_BackupActiveRegions(void);
u8 Save_ReadMainState(u32), Save_WriteMainState(u32), Save_WriteContinuation(u32);
u32 Save_CopySelectedStateToAlternateSlot(void)
{
    u32 slot = gSessionState->slot, other = 1 - slot;
    Save_SnapshotContinuation();
    if (!Save_ReadMainState(slot))
        return 0;
    GameState_BackupActiveRegions();
    if (!Save_WriteMainState(other))
        return 0;
    if (!Save_WriteContinuation(other))
        return 0;
    return 1;
}
