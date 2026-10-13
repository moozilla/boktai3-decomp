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
struct Event {
  u8 pad[0x14];
  u16 code, value;
};
extern struct Event gUnk_03005390;
void sub_08035320(struct Engine *);
void sub_0803548C(struct Engine *);
void sub_08035438(struct Engine *);
void sub_080356AC(struct Engine *, u8);
void sub_080340AC(struct Engine *);
int sub_082238F0(int);
// MATCHING_CONCESSION: register-pinning
void sub_08035D58(u8 command) {
  register int key asm("r3") = command;
  register int saved asm("r1") = key;
  struct Engine *p = gUnk_02000484;
  if (!p)
    goto done;
  if (key == 0x41)
    goto done;
  if (key > 0x41)
    goto high;
  if (key == 0x30)
    goto disconnected;
  if (key > 0x30)
    goto lowMore;
  goto done;
lowMore:
  if (key == 0x32)
    goto removed;
  asm(""
      : "+r"(key)); /* Range switch retains the original strict comparison
                       boundary. */
  switch (key) {
  case 0x32 ... 0x7FFFFFFF:
    break;
  default:
    goto added;
  }
  if (key == 0x33)
    goto disconnected;
  if (key == 0x40)
    goto lost;
  goto done;
high:
  if (key == 0x50)
    goto receive;
  if (key > 0x50)
    goto last;
  if (key == 0x43)
    goto done;
  asm("" : "+r"(key));
  switch (key) {
  case 0x43 ... 0x7FFFFFFF:
    break;
  default:
    goto reset;
  }
  if (key == 0x44)
    goto stop;
  goto done;
last:
  if (key == 0xF1)
    goto error;
  if (key > 0xF1)
    goto lastMore;
  if (key == 0xF0)
    goto error;
  goto done;
lastMore:
  if (saved == 0xF2)
    goto error;
  if (saved == 0xFF)
    goto error;
  goto done;

added: {
  register u8 *slots asm("r2") = &p->selected;
  register int incoming asm("r1") = (u8)gUnk_03005390.code;
  *slots |= incoming;
}
  goto done;
removed: {
  register u8 *slots asm("r2") = &p->selected;
  register int incoming asm("r1") = (u8)gUnk_03005390.code;
  *slots &= ~incoming;
}
  goto done;
disconnected: {
  register u8 *slots asm("r3") = &p->selected;
  register struct Event *event asm("r2") = &gUnk_03005390;
  int zero;
  int remaining;
  register int incoming asm("r1") = (u8)event->code;
  register int selected asm("r0") = *slots & ~incoming;
  zero = 0;
  *slots = selected;
  ((u8 *)p)[0xE9] = 1;
  sub_080356AC(p, (u8)event->code);
  remaining = p->scratch[3];
  if (remaining != 1)
    goto done;
  sub_082238F0(1);
  {
    register int op asm("r0") = 29;
    asm("" : "+r"(op));
    p->flag = zero;
    p->operation = op;
    p->ticks = zero;
  }
  p->pending = remaining;
}
  goto done;
lost: {
  int remaining;
  sub_080356AC(p, (u8)gUnk_03005390.code);
  remaining = p->scratch[3];
  if (remaining != 1)
    goto done;
  sub_082238F0(1);
  setOperation(p, 29, 0);
  p->pending = remaining;
}
  goto done;
reset:
  sub_080340AC(p);
  goto done;
stop:
  setOperation(p, 30, 0);
  goto done;
error:
  if (p->operation != 28)
    setOperation(p, 28, 0);
receive:
  gUnk_020000EC = 0;
done:;
}
