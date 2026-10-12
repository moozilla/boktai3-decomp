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
void sub_0802D688(struct Manager *m, struct Object *p) {
  if (p->next)
    p->next->prev = p->prev;
  else
    m->head = p->prev;
  if (p->prev)
    p->prev->next = p->next;
}
