// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Engine {
  u8 pad0[0x18];
  u8 pad18, bits, pad1a, entered, flag, pad1d, state, flag1f, mode, index;
  u16 mask;
  s8 scroll;
  u8 selected, allowed, count, ids[8], active, flag31, pending, flag33, phase,
      flag35, flag36, pad37;
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
extern struct Engine *gUnk_02000484;
extern u8 gUnk_020000EC;
void sub_08219DD8(void *, int);
#define gRfuLinkStatus gUnk_03006A80
#define gRfuSlotStatusUNI gUnk_03006A60
#define gRfuSlotStatusNI gUnk_03006A70
#define rfu_NI_setSendData sub_08246CC8
#define rfu_clearSlot sub_08246AAC
#include "rfu_compat.h"
static inline void setOperation(struct Engine *p, int op, int zero) {
  p->flag = zero;
  p->operation = op;
  p->ticks = zero;
  p->pending = 1;
}
void sub_0824720C(u8);
void sub_08224E0C(void *, u8 *);
int sub_0803536C(struct Engine *p) {
  register u8 *scratch asm("r9") = p->scratch;
  int result = 0;
  int slot;
  {
    register struct RfuSlotStatusUNI **slots asm("r1") = gUnk_03006A60;
    asm("" : "+r"(slots));
    slot = p->index;
    if (slots[slot]->recv.newDataFlag) {
      u8 *packet = (u8 *)p + 0x90;
      register u8 *previous asm("r8");
      u8 *sequence;
      register int last asm("r6");
      register int next asm("r1");
      register int received asm("r3");
      int i;
      register u16 *flags asm("r3");
      u8 *changed;
      sub_0824720C(slot);
      {
        register u8 *initial asm("r0") = (u8 *)p + 0xD0;
        register u8 *incoming asm("r2");
        last = *initial;
        next = last + 1;
        incoming = (u8 *)p + 0x94;
        asm("" : "+r"(incoming));
        next = (u8)next;
        received = *incoming;
        previous = initial;
        sequence = incoming;
      }
      if (next == received) {
        sub_08224E0C(p->message, sequence);
      } else {
        register int old asm("r0") = (u8)last;
        register u8 *counter asm("r1");
        asm("" : "+r"(old));
        if (old == received)
          counter = (u8 *)p + 0xD4;
        else
          counter = (u8 *)p + 0xD8;
        asm("" : "+r"(counter));
        (*counter)++;
      }
      {
        register int value asm("r0") = *sequence;
        register int zero asm("r1") = 0;
        asm("" : "+r"(value));
        *previous = value;
        (*packet)++;
        packet[1] = zero;
        i = zero;
        flags = &p->flags;
        changed = &p->changed;
      }
      do {
        register u8 *read asm("r6") = scratch;
        asm("" : "+r"(read));
        if ((read[2] >> i) & 1)
          packet[1]++;
      } while (++i <= 4);
      {
        register int value asm("r0") = *flags;
        register int zero asm("r1") = 0;
        asm("" : "+r"(value), "+r"(zero));
        *(u16 *)(packet + 2) = value;
        value = 65535;
        asm("" : "+r"(value));
        *flags = value;
        *changed = zero;
      }
      result = 1;
    }
  }
  return result;
}
