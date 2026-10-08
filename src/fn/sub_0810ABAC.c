#include "global.h"

void sub_08138154(u8 *, u32, void (*)(void));
void sub_0810AD50(void);

void sub_0810ABAC(u8 *p)
{
    sub_08138154(p, 0x11, sub_0810AD50);
}
