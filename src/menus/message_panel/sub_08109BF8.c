#include "engine/shared/global.h"

/* Transfer bookkeeping prefix for the Registry1745 session. */
struct Registry1745TransferState {
    u8 unknown0[0x84];
    u16 counters84[2];
    u8 unknown88[4];
    u16 counters8C[2];
    u8 unknown90[4];
    u32 counter94;
    u32 counters98[2];
};

void Registry1745_ResetTransferCounters(struct Registry1745TransferState *transfer)
{
    s32 i;
    for (i = 0; i < 2; i++) {
        transfer->counters84[i] = 0;
        transfer->counters8C[i] = 0;
        transfer->counters98[i] = 0;
    }
    transfer->counter94 = 0;
}
