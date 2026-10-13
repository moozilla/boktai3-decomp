#include "global.h"
// MATCHING_CONCESSION: register-pinning
void sub_0811D00C(void*,s16*);
void sub_0811B164(void*,int,int,int);
void sub_0803C0A4(u8 *p)
{
    s16 vector[3];
    register s16 *q asm("r5")=vector;
    asm("" : "+r"(q));
    sub_0811D00C(p,vector);
    sub_0811B164(p+0x860,vector[0],q[1],q[2]);
}
