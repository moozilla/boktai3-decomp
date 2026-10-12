#include "global.h"
// MATCHING_CONCESSION: register-pinning
/* Clear the four boot-state words at the start of external RAM. */
extern u32 gUnk_02000000[];
void sub_08001590(void) {
 u32 *start=gUnk_02000000;register u32 zero asm("r2")=0;u32 *p=start+3;
 do {*p=zero;p--;}while((s32)p>=(s32)start);
}
