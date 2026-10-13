#include "palette_controller.h"

u32 sub_080036C0(void)
{
    return gUnk_02000468 ? gUnk_02000468->transition.level : 0x40;
}
