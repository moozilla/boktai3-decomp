#include "global.h"
struct Manager {
    u8 pad[0x1c];
    u16 mode;
};
extern struct Manager *gUnk_02000298;

s32 sub_081D3704(void)
{
    struct Manager *m = gUnk_02000298;
    s32 i;
    if (m == NULL)
        return -1;
    m->mode = 2;
    for (i = 0; i < 40; i++)
        ;
    return 0;
}
