#include "global.h"

#ifdef NONMATCHING
// fuzzy: 71.7%
// ROM returns 0x02020404 as (0x0401FB14 - (0x0401FB14 - 0x0201F400)) + 0x1004, which looks like a difference of two addresses off one symbol; source expression unknown.
void *ScriptProc_Start(u16 key, u16 context);
void LZ77UnCompWram(const void *src, void *dest);
extern u16 gUnk_03001548;

void *sub_08216E54(u16 id)
{
    u8 *res = ScriptProc_Start(0xC091, id);
    if (res == NULL)
        return NULL;
    if ((res[0] == 'M' && res[1] == 'P') || *(u32 *)res == 0x005E8CC5)
        return res;
    if (gUnk_03001548 != id) {
        gUnk_03001548 = id;
        LZ77UnCompWram(res, (void *)0x02020400);
        return (void *)0x02020404;
    }
    return (void *)0x02020404;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08216E54);
#endif
