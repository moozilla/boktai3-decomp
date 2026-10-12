#include "global.h"
struct Object {
  u8 pad0[0x5c];
  u8 motion[0x196];
  u8 registered;
};
int sub_08003C00(void *);
void sub_08003C10(void *);
int sub_0802D914(struct Object *p) {
  int result;
  if (p->registered) {
    sub_08003C10(p->motion);
    p->registered = 0;
    result = 0;
  } else
    result = -1;
  return result;
}
