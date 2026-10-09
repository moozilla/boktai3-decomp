#include "global.h"
void sub_08220C8C(void *,u32,u32,u32,u32);void sub_0820F1A0(void);
u32 sub_0820F298(u8 *p) {
 u16 n=*(u16 *)(p+0x12c);u32 z=0;
 if(*(s16 *)(p+0x12c)>0) {*(u16 *)(p+0x12c)=n-1;}
 else {
 sub_08220C8C(p+0x10c,*(u32 *)(p+0x170),14,p[0x132],z);
 *(void (**)(void))(p+0x108)=sub_0820F1A0;
 }
}
