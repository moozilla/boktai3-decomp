#include "global.h"
extern u8 *gUnk_02000710; extern u32 gUnk_08E60F48[];
u32 *sub_0822E5F4(u32 i) { return &gUnk_08E60F48[*(s16 *)(gUnk_02000710 + i * 2 + 0x4c0)]; }
