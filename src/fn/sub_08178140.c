#include "global.h"
extern u8 *gUnk_02000214;
void sub_08168A0C(void *);
void sub_08168784(void *);
void sub_081685B4(void *);
void sub_08178140(void) {
 if(gUnk_02000214 && *(u32 *)(gUnk_02000214+0xa40)) {
 sub_08168A0C(gUnk_02000214); sub_08168784(gUnk_02000214);
 if(gUnk_02000214[0xa5e]<=2) sub_081685B4(gUnk_02000214);
 }
}
