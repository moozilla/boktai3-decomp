#include "global.h"
static inline u8 start(u8 *p){u8 *flag=p+0x9a;s32 zero=0;*flag=1;if(p[0xb0]){p[0xb0]=zero;p[0xaf]=zero;return 1;}return 0;}
void sub_0819D718(u8 *p){u8 *q=p;if(start(q)){s32 kind=2;u8 *field=p+0xaa;*field=kind;}if(p[0xaf])q[0x383]=1;}
