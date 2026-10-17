#include "global.h"

#include "engine.h"
void sub_081BBDE0(u8 *);
void sub_081BBE20(u8 *);
void sub_081BBE88(u8 *);
void sub_081BC1E4(u8 *);
void sub_081BB538(u8 *);
/* Sets up the Solar Bank screen (four sub-inits), keeps -e at +0x6FC and
   runs sub_081BB538; called by SolarBank_Create (tentative). */
u32 SolarBank_Init(u8 *p)
{
    u32 e;
    sub_081BBDE0(p);
    sub_081BBE20(p);
    sub_081BBE88(p);
    sub_081BC1E4(p);
    if ((e = Script_SeekToKeyword('e')) != 0)
        *(u32 *)(p + 0x6FC) = Script_GetValue();
    else
        *(u32 *)(p + 0x6FC) = e;
    sub_081BB538(p);
    return 0;
}
