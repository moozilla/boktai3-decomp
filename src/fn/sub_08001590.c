#include "global.h"
#define Boot_ClearReservedWords sub_08001590
// Provisional name: Boot_ClearReservedWords
/* Clear the four boot-state words at the start of external RAM. */
extern u32 gUnk_02000000[];
void Boot_ClearReservedWords(void) {
 u32 *start=gUnk_02000000;u32 zero=0;u32 *p=start+3;
 do {*p=zero;p--;}while((s32)p>=(s32)start);
}
