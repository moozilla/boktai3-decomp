#include "global.h"
void sub_08219DD8(void*,u32);
void sub_0820D990(void*,u32,u32,u32,u32);
void sub_0820D9A4(void*,u32,u32,s32);void sub_0820D9B4(void*,u32,u32,u32);
void sub_0820D9BC(void*,void*);void sub_0820D9AC(void*,u32,u32);
void sub_0820D23C(void);
void sub_0820D934(u8 *p)
{
 sub_08219DD8(p,0x38);
 sub_0820D990(p,0,0x3c,0,0);
 sub_0820D9A4(p,0x848f,0,-1);
 sub_0820D9B4(p,0,0,0);
 sub_0820D9BC(p,sub_0820D23C);
 sub_0820D9AC(p,0,0);
 *(u32*)(p+0xc)=0;
}
