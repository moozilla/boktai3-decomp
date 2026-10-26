#include "global.h"
#include "ram.h"

void sub_08225F40(void)
{
    if (gActiveCamera != NULL)
        *(u16 *)(gActiveCamera + 0x54) = 0;
}
