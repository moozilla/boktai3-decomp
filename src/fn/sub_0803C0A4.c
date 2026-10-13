#include "global.h"
void sub_0811D00C(void*,s16*);
void sub_0811B164(void*,int,int,int);
void sub_0803C0A4(u8 *p)
{
    s16 vector[3];
    s16 *q=vector;
    sub_0811D00C(p,vector);
    sub_0811B164(p+0x860,vector[0],q[1],q[2]);
}
