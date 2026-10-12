#include "global.h"
struct Object {
  u8 active;
  u8 pad1[0x5b];
  int id;
  u8 pad60[0x288];
  struct Object *next, *prev;
};
struct Manager {
  u8 pad[0x1c];
  struct Object *head;
};
struct Object *sub_0802D6D0(struct Manager *m, int id) {
  struct Object *p = m->head;
  while (p) {
    if (p->id == id)
      return p;
    p = p->prev;
  }
  return 0;
}
