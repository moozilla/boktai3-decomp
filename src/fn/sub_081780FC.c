#include "global.h"
extern u8 *gUnk_02000214;
void sub_08168850(void *);
void sub_081686FC(void *);
void sub_081685B4(void *);
void sub_081780FC(void) {
 if(gUnk_02000214 && *(u32 *)(gUnk_02000214+0xa40)) {
 sub_08168850(gUnk_02000214); sub_081686FC(gUnk_02000214);
 if(gUnk_02000214[0xa5e]<=2) sub_081685B4(gUnk_02000214);
 }
}
