#include "global.h"
struct S {
    u8 pad[4];
    u8 f4;
};
extern struct S gUnk_03005390;
void RfuManager_RequestStopMode(void)
{
    gUnk_03005390.f4 = 0x15;
}
