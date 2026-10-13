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
  u32 frameCounter;
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
extern u32 gUnk_020000F0, gUnk_0300546C, gUnk_03005408;
void sub_0822C1C8(void);
void sub_0822C1F0(void);
void sub_08219F94(void *);
void sub_08219D38(void *);
int sub_082238F0(int);
void sub_080364C0(struct Engine *);
void sub_08036538(struct Engine *);
void sub_08036568(struct Engine *);
void sub_08036678(struct Engine *);
void sub_0803670C(struct Engine *);
void sub_08036778(struct Engine *);
int sub_080367B4(struct Engine *p) {
  sub_0822C1C8();
  gUnk_020000F0 = p->flag33;
  switch (p->phase) {
  case 0:
    if (p->active)
      sub_080364C0(p);
    else
      sub_08036538(p);
    break;
  case 1:
    if (p->active)
      sub_08036568(p);
    else
      sub_08036678(p);
    break;
  case 2:
    if (p->active)
      sub_0803670C(p);
    else
      sub_08036778(p);
    break;
  }
  if (p->flag3d) {
    int zero = 0;
    p->flag3d = zero;
    sub_082238F0(1);
    setOperation(p, zero, zero);
  }
  if (p->pad1d) {
    gUnk_0300546C = 0;
    gUnk_02000484 = 0;
    sub_08219F94(p);
    sub_08219D38(p);
    sub_0822C1F0();
  } else {
    if ((u16)(p->operation - 9) <= 17)
      gUnk_03005408 = 1;
    p->ticks++;
    p->frameCounter++;
  }
  return 0;
}
