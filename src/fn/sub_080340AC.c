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
    int zero = 0;
    int i;
    u8 *ids;
    u8 *global;
    int byteZero;
    int secondZero, secondByteZero, one, clearZero;
    u8 *active;
    p->flag = zero;
    p->bits = zero;
    p->pad1a = zero;
    p->state = zero;
    p->flag1f = zero;
    p->mode = zero;
    p->index = zero;
    byteZero = 0;
    p->mask = zero;
    p->scroll = byteZero;
    p->selected = byteZero;
    p->allowed = byteZero;
    p->count = byteZero;
    global = &gWirelessSessionState;
    clearZero = 0;
    i = 7;
    ids = &p->ids[7];
    do {
        *ids = clearZero;
        ids--;
        i--;
    } while (i >= 0);
    active = &p->active;
    secondZero = 0;
    one = 1;
    *active = one;
    p->flag31 = secondZero;
    p->phase = secondZero;
    p->flag35 = secondZero;
    p->flag36 = secondZero;
    p->flag = secondZero;
    secondByteZero = 0;
    p->operation = secondZero;
    p->ticks = secondZero;
    p->pending = one;
    p->value = secondZero;
    p->flag3c = secondByteZero;
    p->flag3d = secondByteZero;
    p->flag3e = secondByteZero;
    p->flag3f = secondByteZero;
    p->timer = secondZero;
    p->flag42 = secondByteZero;
    p->flag43 = secondByteZero;
    p->pointer = 0;
    *global = secondByteZero;
    p->flagFB = secondByteZero;
    p->phase = secondByteZero;
    p->head = 0;
    p->tail = 0;
    Memory_Clear(p->scratch, 64);
    Memory_Clear(&p->number, 4);
    Memory_Clear(p->work, 16);
}
