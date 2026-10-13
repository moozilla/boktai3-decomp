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
void sub_0803548C(struct Engine *argument) {
  register struct Engine *p asm("r5") = argument;
  register u8 *scratch asm("r10") = p->scratch;
  register int i asm("r4");
  register u8 *packet asm("r6");
  register u8 *seen asm("r8");
  p->flag3e = 0;
  i = 0;
  packet = (u8 *)p + 0x80;
  do {
    register int offset asm("r2") = i << 2;
    register int bit asm("r1") = 2 << i;
    register int test asm("r0");
    register u8 *initialSeen asm("r3");
    asm("" : "+r"(offset));
    {
      register u8 *read asm("r3") = scratch;
      asm("" : "+r"(read));
      test = read[2] & bit;
    }
    {
      register int offset asm("r3") = 0xE6;
      asm("" : "+r"(offset));
      initialSeen = (u8 *)(offset + (u32)p);
    }
    asm("" : "+r"(initialSeen), "+r"(test));
    seen = initialSeen;
    if (test) {
      {
        register struct RfuSlotStatusUNI **entry asm("r0") =
            (struct RfuSlotStatusUNI **)(offset + (u32)gUnk_03006A60);
        asm("" : "+r"(entry));
        if ((*entry)->recv.newDataFlag) {
          u8 *previous;
          register int old asm("r3");
          register int incoming asm("r2");
          register int next asm("r0");
          register int zero asm("r9");
          bit |= *initialSeen;
          zero = 0;
          asm("" : "+r"(zero));
          *initialSeen = bit;
          sub_0824720C(i);
          {
            register u8 *initial asm("r1") = (u8 *)p + 0xD0;
            old = initial[i];
            next = (u8)(old + 1);
            incoming = packet[0];
            previous = initial;
          }
          if (next != incoming) {
            {
              register int previousValue asm("r0") = old;
              asm("" : "+r"(previousValue));
              if (previousValue == incoming) {
                {
                  register u8 *counter asm("r1") = (u8 *)p + 0xD4;
                  asm("" : "+r"(counter));
                  counter += i;
                  asm("" : "+r"(counter));
                  (*counter)++;
                }
                {
                  register int value asm("r2") = zero;
                  asm("" : "+r"(value));
                  *(u16 *)(packet + 2) = value;
                }
              } else if ((u32)previousValue < (u32)incoming) {
                {
                  register u8 *counter asm("r1") = (u8 *)p + 0xDC;
                  asm("" : "+r"(counter));
                  counter += i;
                  asm("" : "+r"(counter));
                  (*counter)++;
                }
                {
                  register int value asm("r3") = zero;
                  asm("" : "+r"(value));
                  *(u16 *)(packet + 2) = value;
                }
              } else {
                {
                  register u8 *counter asm("r1") = (u8 *)p + 0xD8;
                  asm("" : "+r"(counter));
                  counter += i;
                  asm("" : "+r"(counter));
                  (*counter)++;
                }
              }
            }
          }
          previous[i] = *packet;
        }
      }
    }
    packet += 4;
  } while (++i <= 3);
  {
    register int got asm("r1") = *seen;
    register int expected asm("r0");
    register int saved asm("r3");
    register int matched asm("r2");
    u8 *sent;
    {
      register u8 *read asm("r2") = scratch;
      asm("" : "+r"(read));
      expected = read[2];
    }
    saved = expected;
    asm("" : "+r"(saved));
    matched = saved;
    asm("" : "+r"(matched));
    matched &= got;
    got = expected;
    asm("" : "+r"(got), "+r"(matched));
    sent = (u8 *)p + 0xE7;
    if (matched == got) {
      saved &= *sent;
      asm("" : "+r"(saved));
      if (saved == matched)
        goto checkSent;
      {
        u8 *counter = (u8 *)p + 0xE4;
        (*counter)++;
      }
    }
    {
      register u8 *read asm("r3");
      register int value asm("r0") = *sent;
      asm("" : "+r"(value));
      read = scratch;
      asm("" : "+r"(read));
      got = read[2];
      value &= got;
      asm("" : "+r"(value));
      if (value != got)
        return;
    }
  checkSent: {
    register u8 *read asm("r2") = seen;
    register int value asm("r0");
    asm("" : "+r"(read));
    value = *read;
    value &= got;
    asm("" : "+r"(value));
    if (value != got) {
      register u8 *counter asm("r1") = (u8 *)p + 0xE5;
      asm("" : "+r"(counter));
      (*counter)++;
    }
  }
  }
}
