#include "global.h"
void sub_081BE7FC(void *);
void sub_081BE8B0(void *);
void sub_081BE730(void *);
void sub_081BEDFC(u8 *p) {
 u8 *a=p+0x1754,*b=p+0x1758;
 switch(b[*a]>>1) {
 case 0: sub_081BE7FC(p); break;
 case 1: sub_081BE8B0(p); break;
 case 2: sub_081BE730(p); break;
 }
}
