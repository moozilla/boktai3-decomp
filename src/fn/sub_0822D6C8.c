#include "global.h"
struct E { u8 b[16]; };
struct G { u8 pad[0x1c0]; struct E e[1]; };
extern struct G *gUnk_02000710;
struct E *sub_0822D6C8(u32 i) { return &gUnk_02000710->e[i]; }
