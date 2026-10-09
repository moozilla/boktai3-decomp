#include "global.h"
void sub_08020D68(void *,int);
static inline u8 test(u8 *p) { if(p[0xb0]) {p[0xb0]=0;p[0xaf]=0; return 1;} return 0; }
void sub_0817B554(u8 *p) {
 if(test(p)) {u32 x=0x10; p[0xaa]=x;}
 if(p[0xaf]) sub_08020D68(p+0x118,1);
 (*(u32 *)(p+0xc4))++;
}
