#include "global.h"
#include "engine/ram.h"
#include "link/wireless/rfu_manager.h"
#include "lib/rfu/rfu_compat.h"
struct Engine {
    u8 pad0[0x18];
    u8 pad18, bits, pad1a, entered, flag, pad1d, state, flag1f, mode, index;
    u16 mask;
    s8 scroll;
    u8 selected, allowed, count, ids[8], active, flag31, pending, flag33, phase, flag35, flag36,
        pad37;
    u16 operation, value;
    u8 flag3c, flag3d, flag3e, flag3f;
    u16 timer;
    u8 flag42, flag43;
    u32 ticks;
    void *pointer;
    u8 pad4c[0xA0];
    u16 flags, backup, history[5];
    u8 changed, padfb[5];
    u16 diff[5];
    u8 pad10a[2];
    u32 key;
    u16 input[5];
    u8 pad11a[0x1A];
    u8 scratch[64];
    u8 message[0x204];
    void *head, *tail;
};
void Memory_Clear(void *, int);
struct ModeState {
    u8 pad0;
    u8 mode;
};
void WirelessController_ResetPeerExchange(struct Engine *p)
{
    p->state = 0;
    p->mask = 0;
    p->flag35 = 0;
    p->flag36 = 0;
    p->flag3c = 0;
    p->flag3d = 0;
    p->flag3e = 0;
    p->flag3f = 0;
    Memory_Clear(p->scratch, 64);
}
