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
void sub_0822C1C8(void);
void *sub_08219C40(int);
void sub_08219DD8(void *, int);
int sub_08228DA0(void);
int sub_080368AC(struct Engine *);
void sub_0821A04C(void *, int, int);
void sub_08219F74(void *);
void sub_08219D38(void *);
extern u32 gUnk_03005308, gUnk_0300546C;
extern const u16 gUnk_0203B400[];
extern u8 gUnk_03003A50;
struct Engine *sub_08036910(void) {
  struct Engine **singleton = &gUnk_02000484;
  struct Engine *existing = *singleton;
  struct Engine *p;
  int size;
  register int random asm("r0");
  if (existing)
    return existing;
  sub_0822C1C8();
  size = 0x38C;
  p = sub_08219C40(size);
  if (!p)
    goto failed;
  asm("" : "+r"(p));
  sub_08219DD8(p, size);
  random = sub_08228DA0();
  {
    register u32 *position asm("r3") = &gUnk_03005308;
    register u32 index asm("r1") = *position + 1;
    register u32 mask asm("r2") = 1023;
    register const u16 *table asm("r2");
    register int term asm("r3");
    register const u8 *saltAddress asm("r1");
    register int byteMask asm("r2");
    register int salt asm("r1");
    register u16 *destination asm("r0");
    asm("" : "+r"(mask));
    index &= mask;
    *position = index;
    index <<= 1;
    table = gUnk_0203B400;
    asm("" : "+r"(table));
    term = *(const u16 *)(index + (u32)table);
    saltAddress = &gUnk_03003A50;
    byteMask = 255;
    asm("" : "+r"(saltAddress), "+r"(byteMask));
    salt = *saltAddress;
    random += term;
    random &= byteMask;
    random <<= 8;
    salt |= random;
    destination = (u16 *)((u8 *)p + 0x6C);
    *destination = salt;
  }
  if (sub_080368AC(p) >= 0) {
    int one;
    sub_0821A04C(p, 0, 0);
    one = 1;
    ((u8 *)p)[0x14] = one;
    ((u8 *)p)[0x16] = one;
    *(u16 *)((u8 *)p + 0x10) = (u32)existing;
    sub_08219F74(p);
    gUnk_0300546C = one;
    *singleton = p;
    return p;
  }
  sub_08219D38(p);
failed:
  return 0;
}
