#include "global.h"
// Tentative result-menu offsets from the contiguous callback family.
struct Drawing {
  u8 bytes[0x60];
};
struct ResultMenu {
  u8 task[0x18];
  u32 timer;
  u8 state, entered;
  u8 pad1e[0x9a];
  struct Drawing draws[22];
  void *onClose, *resource;
  int windows[4];
};
extern struct ResultMenu *gUnk_02000104;
extern void *gUnk_08605814[];
extern u16 gUnk_03005260[], gUnk_03004BD8;
void sub_0822AFE8(int);
void sub_0822B2F8(int);
void sub_08249238(void *);
void sub_0821A0C0(void *);
int sub_08036BEC(void);
void sub_08033778(int);
void sub_0824923C(void *, void *);
void sub_082195E0(void *);
void sub_08214514(void *);
void sub_0803B314(struct ResultMenu *p) {
  if (p->entered)
    p->entered = 0;
  if (p->timer > 29 && (gUnk_03005260[1] & 0x30F)) {
    sub_0822AFE8(4);
    sub_0822B2F8(221);
    if (p->onClose)
      sub_08249238(p->onClose);
    sub_0821A0C0(p);
  } else
    p->timer++;
}