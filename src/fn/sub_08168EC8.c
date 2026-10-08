#include "global.h"
u32 sub_08168E38(void *);
void sub_08168C90(void *);
u32 sub_08168EC8(void *p)
{
    if (sub_08168E38(p)) {
        sub_08168C90(p);
        return 1;
    }
    return 0;
}
