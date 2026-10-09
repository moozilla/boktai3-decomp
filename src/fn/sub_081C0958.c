#include "global.h"
void sub_081C32C8(void *, int, s16 *, int, int, int, int);
void sub_081C09A0(void);
void sub_081C0958(u8 *p) {
 struct V {s16 x,y,z;} v; s16 *q=(s16 *)&v; int a=0x36,b=0x52,z=0;
 v.x=a; q[1]=b; q[2]=z;
 sub_081C32C8(*(void **)(p+0x30),0,(s16 *)&v,-1,0x1,*(s16 *)(p+0x5C),0x3C);
 *(void (**)(void))(p+0x50)=sub_081C09A0; *(u32 *)(p+0x54)=z;
}
