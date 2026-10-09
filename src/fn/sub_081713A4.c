#include "global.h"
void sub_08170D78(void *);
void sub_081719A8(void);
void sub_08163EB8(void *, void (*)(void), int);
void sub_0816CDB8(void *, int, int);
void sub_0816B654(void *, int, int);
void sub_08165B7C(void *, int, int);
void sub_08170F70(void *);
void sub_081663A0(void *, int, int, int, int);
void sub_08166B7C(void *, int);
void sub_081713A4(u8 *p, int flag) {
 u8 *q;
 sub_08170D78(p); sub_08163EB8(p,sub_081719A8,1);
 q=p+0x4384; sub_0816CDB8(q,1,0);
 if(flag) sub_0816B654(q,0,0);
 sub_08165B7C(p,6,1); sub_08170F70(p); sub_081663A0(p,1,0,0,0); sub_08166B7C(p,3);
}
