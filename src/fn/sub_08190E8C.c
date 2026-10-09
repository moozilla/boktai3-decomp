#include "global.h"
void sub_08219728(void *, void *, int);
struct Flags
{
  u8 pad[0x71dc];
  u8 flags[52];
  u8 done[52];
};
struct Entry
{
  u32 flags;
  u8 pad[0x5c];
};
struct Entries
{
  u8 pad[0x5444];
  struct Entry items[52];
};
void sub_08190E8C(u8 *p, int i)
{
  u8 **flag_pointer;
  u8 *q;
  q = p + 0x71dc;
  q += i;
  flag_pointer = &q;
  if (!(*(*flag_pointer)))
  {
    u32 off = i * 0x60;
    ((struct Entries *) p)->items[i].flags &= ~1;
    sub_08219728(p + (off + 0x543c), p + 0x541c, 13);
    (*(*flag_pointer))++;
    ((struct Flags *) p)->done[i] = 1;
  }
}
