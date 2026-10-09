#include "global.h"
s32 sub_08165A30(void *);void sub_0822B2F8(s32);void sub_0822E18C(s32);void sub_0817147C(void *);
void sub_08171D08(u8 *p){s32 r=sub_08165A30(p);if(r==0){sub_0822B2F8(0xde);sub_0817147C(p);}else if(r==1){sub_0822B2F8(0xdd);sub_0822E18C(0);p[0x4388]=0;p[0x4389]=0;p[0x438a]=0;sub_0817147C(p);}}
