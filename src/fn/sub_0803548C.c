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
  struct Engine *p = argument;
  u8 *scratch = p->scratch;
  int i;
  u8 *packet;
  register u8 *seen asm("r8");
  p->flag3e = 0;
  i = 0;
  packet = (u8 *)p + 0x80;
  do {
    int offset = i << 2;
    register int bit asm("r1") = 2 << i;
    int test;
    u8 *initialSeen;
    {
      register u8 *read asm("r3") = scratch;
      test = read[2] & bit;
    }
    {
      register int offset asm("r3") = 0xE6;
      asm("" : "+r"(offset));
      initialSeen = (u8 *)(offset + (u32)p);
    }
    seen = initialSeen;
    if (test) {
      {
        struct RfuSlotStatusUNI **entry =
            (struct RfuSlotStatusUNI **)(offset + (u32)gUnk_03006A60);
        if ((*entry)->recv.newDataFlag) {
          u8 *previous;
          int old;
          int incoming;
          int next;
          register int zero asm("r9");
          bit |= *initialSeen;
          zero = 0;
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
              int previousValue = old;
              asm("" : "+r"(previousValue));
              if (previousValue == incoming) {
                {
                  register u8 *counter asm("r1") = (u8 *)p + 0xD4;
                  counter += i;
                  (*counter)++;
                }
                {
                  register int value asm("r2") = zero;
                  *(u16 *)(packet + 2) = value;
                }
              } else if ((u32)previousValue < (u32)incoming) {
                {
                  register u8 *counter asm("r1") = (u8 *)p + 0xDC;
                  counter += i;
                  (*counter)++;
                }
                {
                  register int value asm("r3") = zero;
                  *(u16 *)(packet + 2) = value;
                }
              } else {
                {
                  register u8 *counter asm("r1") = (u8 *)p + 0xD8;
                  counter += i;
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
    int got = *seen;
    int expected;
    register int saved asm("r3");
    register int matched asm("r2");
    u8 *sent;
    {
      register u8 *read asm("r2") = scratch;
      expected = read[2];
    }
    saved = expected;
    matched = saved;
    matched &= got;
    got = expected;
    sent = (u8 *)p + 0xE7;
    if (matched == got) {
      saved &= *sent;
      if (saved == matched)
        goto checkSent;
      {
        u8 *counter = (u8 *)p + 0xE4;
        (*counter)++;
      }
    }
    {
      u8 *read;
      int value = *sent;
      read = scratch;
      got = read[2];
      value &= got;
      if (value != got)
        return;
    }
  checkSent: {
    register u8 *read asm("r2") = seen;
    int value;
    asm("" : "+r"(read));
    value = *read;
    value &= got;
    asm("" : "+r"(value));
    if (value != got) {
      u8 *counter = (u8 *)p + 0xE5;
      (*counter)++;
    }
  }
  }
}
