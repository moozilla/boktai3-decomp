#include "global.h"
#include "ram.h"
#include "rfu_manager.h"
#include "rfu_compat.h"
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
    u8 pad4c[0x58];
    u32 number;
    u8 work[16];
    u8 padb8[0x34];
    u16 flags, backup, history[5];
    u8 changed, flagFB, padfc[4];
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
void WirelessController_ResetSession(struct Engine *p)
{
    int i;
    p->flag = 0;
    p->bits = 0;
    p->pad1a = 0;
    p->state = 0;
    p->flag1f = 0;
    p->mode = 0;
    p->index = 0;
    p->mask = 0;
    p->scroll = 0;
    p->selected = 0;
    p->allowed = 0;
    p->count = 0;
    for (i = 0; i < 8; i++)
        p->ids[i] = 0;
    p->active = 1;
    p->flag31 = 0;
    p->phase = 0;
    p->flag35 = 0;
    p->flag36 = 0;
    p->flag = 0;
    p->operation = 0;
    p->ticks = 0;
    p->pending = 1;
    p->value = 0;
    p->flag3c = 0;
    p->flag3d = 0;
    p->flag3e = 0;
    p->flag3f = 0;
    p->timer = 0;
    p->flag42 = 0;
    p->flag43 = 0;
    p->pointer = 0;
    gWirelessSessionState = 0;
    p->flagFB = 0;
    p->phase = 0;
    p->head = 0;
    p->tail = 0;
    Memory_Clear(p->scratch, 64);
    Memory_Clear(&p->number, 4);
    Memory_Clear(p->work, 16);
}
