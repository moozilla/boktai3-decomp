#include "global.h"
#include "background_controller.h"

u32 sub_08001AD0(struct Background *p)
{
    gUnk_02000000[p->layer] = 0;
    return 0;
}
