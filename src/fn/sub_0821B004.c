#include "global.h"
struct T {
    u8 p[0xc];
    u8 *f0c;
};
extern struct T gUnk_02000438;
extern const u8 gUnk_08614184[];
u8 *Script_DecodePacket(u8 *, u32 *);
void Script_RunBlock(u8 *, const u8 *);
void Script_RunMainBlock(void)
{
    u32 x;
    Script_RunBlock(Script_DecodePacket(gUnk_02000438.f0c, &x), gUnk_08614184);
}
