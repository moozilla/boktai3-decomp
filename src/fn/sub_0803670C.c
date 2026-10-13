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
void sub_08223BC0(int);
void sub_08246644(void);
void sub_082456E0(void);
void sub_08223580(int);
void sub_08035F5C(struct Engine *);
void sub_08035FF4(struct Engine *);
int sub_0824923C(void *, void *);
extern void *gUnk_086056EC[];
// MATCHING_CONCESSION: register-pinning
void sub_0803670C(struct Engine *argument) {
  register struct Engine *p asm("r5") = argument;
  volatile u8 *busy = &gUnk_020000EC;
  u8 *lost;
  while (*busy == 1)
    ;
  sub_08223BC0(0);
  lost = &p->flag35;
  if (*lost) {
    sub_08246644();
    sub_082456E0();
    *lost = 0;
  }
  {
    int mode = gUnk_03006A80->parentChild;
    if (mode == 1) {
      gUnk_020000EC = mode;
      sub_08223580(1);
    }
  }
  sub_08035F5C(p);
  sub_08035FF4(p);
  sub_0824923C(p, gUnk_086056EC[p->operation]);
}
