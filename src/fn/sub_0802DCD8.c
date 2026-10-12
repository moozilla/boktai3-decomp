#include "global.h"
struct Entry {
  u8 kind;
  s8 stride;
  u16 animation;
};
struct Actor {
  u8 pad[0x2b8];
  struct Entry *entries;
};
int sub_0802DBD4(struct Actor *, int, int, int, int, int);
int sub_0802DB78(struct Actor *, int, int, int, int, int);
int sub_0802DC7C(struct Actor *, int, int, int, int, int);
int sub_0802DC18(struct Actor *, int, int, int, int, int);
int sub_0802DCB8(struct Actor *, int, int, int, int, int);
int sub_0802DCD8(struct Actor *p, int id, int a, int b, int c) {
  struct Entry *entry = &p->entries[id];
  int result;
  if (entry->kind == 1)
    result = sub_0802DBD4(p, entry->animation, a, entry->stride, b, c);
  else if (entry->kind == 3)
    result = sub_0802DB78(p, entry->animation, a, entry->stride, b, c);
  else if (entry->kind == 2)
    result = sub_0802DC7C(p, entry->animation, a, entry->stride, b, c);
  else if (entry->kind == 4)
    result = sub_0802DC18(p, entry->animation, a, entry->stride, b, c);
  else
    result = sub_0802DCB8(p, entry->animation, a, entry->stride, b, c);
  return result;
}
