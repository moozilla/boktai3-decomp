#include "global.h"
#define SpritePool_SetResource sub_08019BB0
extern u32 gUnk_020000B0;
void sub_082151E4(void *,u32);
void SpritePool_SetResource(u8 *p,u32 id) {
 if(gUnk_020000B0)sub_082151E4(p+0x38,id);
}
