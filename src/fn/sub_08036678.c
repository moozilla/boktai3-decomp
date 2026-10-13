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
void sub_0824722C(int);
void sub_0803441C(struct Engine *);
void sub_0803451C(struct Engine *);
int sub_0824923C(void *, void *);
extern void *gUnk_086056EC[];
void sub_080341B8(struct Engine *);
void sub_080341E0(struct Engine *);
void sub_0803559C(struct Engine *);
void sub_08223580(int);
void sub_0824555C(void);
void sub_0824714C(int, void *, int);
void sub_08246968(void);
extern u32 gUnk_03005250;
extern u16 gUnk_03005274[];
// MATCHING_CONCESSION: register-pinning
static inline void publishInputs(struct Engine *p) {
  int enabled = p->flag31;
  if (enabled) {
    register int i asm("r5");
    int valid;
    int mask;
    u16 *output;
    int invalid;
    register u16 *history asm("r4");
    gUnk_03005250 = 1;
    i = 0;
    {
      int initial = 0x2000;
      valid = initial;
    }
    mask = 4095;
    output = gUnk_03005274;
    {
      register int initial asm("r1") = 65535;
      asm("" : "+r"(initial));
      invalid = initial;
    }
    history = p->history;
    do {
      register int value asm("r2") = *history;
      register int test asm("r1") = valid;
      register int result asm("r1");
      if (value & test) {
        result = mask;
        result &= value;
      } else
        result = *output | invalid;
      *output = result;
      output++;
      history++;
    } while (++i <= 4);
  } else
    gUnk_03005250 = enabled;
}
int sub_08036678(struct Engine *p) {
  u8 *ready = &p->flag31;
  {
    int zero = 0;
    *ready = zero;
    sub_08223BC0(zero);
  }
  sub_08246968();
  sub_080341B8(p);
  sub_080341E0(p);
  {
    void **handlers = gUnk_086056EC;
    int op = p->operation;
    sub_0824923C(p, handlers[op]);
  }
  publishInputs(p);
}
