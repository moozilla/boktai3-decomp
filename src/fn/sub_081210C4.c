#include "global.h"

void sub_08120DAC(u8 *);

void RegistryA8CB_ResetUpdateState(u8 *obj)
{
    *(u16 *)(obj + 0x8BA) = 0;
    sub_08120DAC(obj);
}
