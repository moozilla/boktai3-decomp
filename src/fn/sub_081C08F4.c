#include "global.h"
void sub_081C32C8(void *, int, s16 *, int, int, int, int);
void sub_081C093C(void);
void sub_081C08F4(u8 *p) {
 struct V {s16 x,y,z;} v; s16 *q=(s16 *)&v; int a=0x70,b=0x48,z=0;
 v.x=a; q[1]=b; q[2]=z;
 sub_081C32C8(*(void **)(p+0x30),0,(s16 *)&v,-1,0x10,*(s16 *)(p+0x5a),0x32);
 *(void (**)(void))(p+0x50)=sub_081C093C; *(u32 *)(p+0x54)=z;
}
