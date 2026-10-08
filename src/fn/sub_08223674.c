#include "global.h"
struct S { u8 pad[4]; u8 f4; u8 f5; u8 f6; u8 f7; u8 pad8[4]; u8 fc; u8 pad2[3]; u8 f10; u8 pad3[0x13]; u8 f24; u8 pad24[3]; u16 a28[4]; u8 f30; u8 pad5[3]; u16 a34[4]; };
extern struct S gUnk_03005390;
void sub_08223674(void)
{
    u8 i;
    gUnk_03005390.f5 = 0;
    gUnk_03005390.f4 = 0;
    gUnk_03005390.f6 = 0xff;
    gUnk_03005390.f7 = 0;
    gUnk_03005390.f10 = 0;
    gUnk_03005390.fc = 0;
    gUnk_03005390.f24 = 0;
    gUnk_03005390.f30 = 0;
    for (i = 0; i < 4; i++) {
        gUnk_03005390.a28[i] = 0;
        gUnk_03005390.a34[i] = 0;
    }
}
