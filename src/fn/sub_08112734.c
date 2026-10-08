#include "global.h"
void sub_08111D44(void *);
void sub_08112144(void *);
void sub_0811CD08(void *);
u32 sub_08112734(u8 *p)
{
    sub_08111D44(p);
    sub_08112144(p);
    sub_0811CD08(p + 0xF60);
    return 1;
}
