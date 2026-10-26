#include "global.h"
#include "ram.h"
struct Manager {
    u8 pad[0x1c];
    u16 mode;
};

s32 sub_081D3704(void)
{
    struct Manager *m = gCloudTask;
    s32 i;
    if (m == NULL)
        return -1;
    m->mode = 2;
    for (i = 0; i < 40; i++)
        ;
    return 0;
}
