#include "global.h"

void sub_08013684(void *);
void sub_08214514(void *);

u32 sub_0800FA40(u32 a, u8 *b)
{
    sub_08013684(b + 0x88);
    sub_08214514(b + 0x40);
}
