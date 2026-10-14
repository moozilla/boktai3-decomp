// COMPILER: old_agbcc
#include "global.h"

extern struct {
    u8 pad[20];
    u16 value;
} gUnk_03005430;

u16 sub_082290D0(void)
{
    return gUnk_03005430.value;
}
