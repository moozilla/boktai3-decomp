#include "global.h"
void sub_081BE960(void *);
void sub_081BEA4C(void *);
void sub_081BEB08(void *);
void sub_081BECA4(u8 *p) {
 u8 *a=p+0x1754,*b=p+0x1758;
 switch(b[*a]>>1) {
 case 0: sub_081BE960(p); break;
 case 1: sub_081BEA4C(p); break;
 case 2: sub_081BEB08(p); break;
 }
}
