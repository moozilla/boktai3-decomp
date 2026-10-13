#include "global.h"
u32 AnimData_FindEntry(u32);
void sub_0813AD48(u8 *p)
{
    *(u32 *)(p + 0x18) = AnimData_FindEntry(0x1C1E);
}
