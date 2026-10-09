#include "global.h"
void sub_081D1A10(void *, int, int, int, int);
int sub_080424FC(void *);
void sub_081D1D80(void *unused, u8 *p) {
 if(p[9]) { p[9]=0; p[7]=0; sub_081D1A10(p,5,2,0,4); }
 if(sub_080424FC(p+0xd4)) p[7]=1;
}
