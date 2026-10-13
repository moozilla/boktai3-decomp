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
// MATCHING_CONCESSION: register-pinning
void sub_08224E0C(void *, u8 *);
void CpuSet(const void *, void *, u32);
void sub_0803559C(struct Engine *argument) {
  struct {
    u32 fill;
    int sequence;
    u8 *waiting;
    u8 *sent;
  } stack;
  register struct Engine *p asm("r5") = argument;
  register u8 *scratch asm("r8") = p->scratch;
  u8 *packet = (u8 *)p + 0x70;
  register u8 *sent asm("r9");
  register int got asm("r0");
  int expected;
  register u8 *read asm("r2");
  {
    register int offset asm("r1") = 0xE7;
    register u8 *initial asm("r1");
    asm("" : "+r"(offset));
    initial = (u8 *)(offset + (u32)p);
    asm("" : "+r"(initial));
    sent = initial;
    got = *initial;
    read = scratch;
    asm("" : "+r"(read));
    expected = read[2];
  }
  got &= expected;
  asm("" : "+r"(got));
  if (got == expected) {
    register int zero asm("r6") = p->flag3e;
    if (zero) {
      if (read[3] > 1) {
        register u8 *waiting asm("r1") = &p->flag36;
        asm("" : "+r"(waiting));
        *waiting = 1;
      }
    } else {
      u8 *waiting;
      register int i asm("r2");
      register u16 *backup asm("r6");
      register u16 *flags asm("r12");
      register u8 *seen asm("r10");
      register int bit asm("r9");
      register u16 *input asm("r3");
      u16 *output;
      stack.sequence = *packet;
      asm("" : : "m"(stack.sequence));
      sub_08224E0C(p->message, packet);
      waiting = &p->flag36;
      *waiting = zero;
      stack.fill = zero;
      CpuSet(&stack.fill, packet, 0x05000004);
      i = 0;
      stack.sent = sent;
      stack.waiting = waiting;
      asm("" : "+m"(stack.sent), "+m"(stack.waiting), "+m"(stack.sequence));
      backup = (u16 *)((u8 *)p + 0xEA);
      {
        register int offset asm("r1") = 0xEC;
        asm("" : "+r"(offset));
        flags = (u16 *)(offset + (u32)p);
      }
      {
        register int offset asm("r0") = 0xE6;
        asm("" : "+r"(offset));
        seen = (u8 *)(offset + (u32)p);
      }
      {
        register int initial asm("r1") = 2;
        asm("" : "+r"(initial));
        bit = initial;
      }
      input = (u16 *)((u8 *)p + 0x82);
      output = (u16 *)(waiting + 0x40);
      do {
        register int mask asm("r1") = bit;
        register u8 *read asm("r5");
        asm("" : "+r"(mask));
        mask <<= i;
        read = scratch;
        asm("" : "+r"(read));
        if (read[2] & mask)
          *output = *input;
        input += 2;
        output++;
      } while (++i <= 3);
      *(u16 *)(packet + 4) = *backup;
      {
        register u16 *read asm("r1") = flags;
        register int value asm("r0");
        asm("" : "+r"(read));
        value = *read;
        *backup = value;
        value = 65535;
        asm("" : "+r"(value));
        *read = value;
      }
      {
        register u8 *read asm("r2") = scratch;
        asm("" : "+r"(read));
        packet[2] = read[2];
      }
      packet[1] = 1;
      i = 0;
      {
        register int sequence asm("r3") = stack.sequence + 1;
        register int bit asm("r5") = 2;
        asm("" : "+r"(sequence), "+r"(bit));
        do {
          register u8 *read asm("r0") = scratch;
          int active;
          register int mask asm("r0");
          asm("" : "+r"(read));
          active = read[2];
          mask = bit << i;
          asm("" : "+r"(mask));
          if (active & mask)
            packet[1]++;
        } while (++i <= 3);
        *packet = sequence;
      }
      {
        int one = 1;
        register u8 *write asm("r2") = stack.sent;
        asm("" : "+r"(write));
        *write = one;
        {
          register u8 *write asm("r5") = seen;
          asm("" : "+r"(write));
          *write = one;
        }
        {
          register u8 *read asm("r2") = scratch;
          asm("" : "+r"(read));
          if (read[3] > 1) {
            register u8 *write asm("r5") = stack.waiting;
            asm("" : "+r"(write));
            *write = one;
          }
        }
      }
    }
  }
}
