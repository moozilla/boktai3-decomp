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
void sub_08036BD4(int);
int sub_08035F5C(struct Engine *p) {
  int flags = p->mask;
  struct RfuLinkStatus **links;
  register int one asm("r6");
  u8 *slot;
  int index;
  if (flags & 2)
    goto failed;
  links = &gUnk_03006A80;
  if ((*links)->linkLossSlotFlag)
    goto failed;
  one = 1;
  {
    int check = one;
    check &= flags;
    asm("" : "+r"(check));
    if (!check)
      goto ready;
  }
  slot = &p->mode;
  rfu_clearSlot(4, *slot);
  if (rfu_NI_setSendData((u8)(one << *slot), 15, (u8 *)p + 0xB8, 12))
    goto sendError;
  index = 0;
  if (!((*links)->connSlotFlag & one)) {
    struct RfuLinkStatus **cursor = links;
    int bit = 1;
    do {
      index++;
      if (index > 3)
        break;
    } while (
        !(((*(struct RfuLinkStatus *volatile *)cursor)->connSlotFlag >> index) &
          bit));
  }
  *slot = index;
  p->mask = (p->mask & 0xFFFE) | 2;
  goto ready;
sendError:
  sub_08036BD4(2);
failed:
  return 0;
ready:
  return 1;
}
