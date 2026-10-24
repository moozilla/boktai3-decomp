#include "global.h"

// MATCHING_CONCESSION: register-pinning
void *ScriptProc_Start(u16 key, u16 context);
void LZ77UnCompWram(const void *src, void *dest);
extern u16 gUnk_03001548;
extern u8 gUnk_02000714[];

void *sub_08216E54(u16 id)
{
    void *result;
    u32 key = id;
    u8 *res = ScriptProc_Start(0xC091, key);
    if (res == NULL)
        return NULL;
    if ((res[0] == 'M' && res[1] == 'P') || *(u32 *)res == 0x005E8CC5)
        return res;
    if (gUnk_03001548 != key) {
        u8 *anchor, *heapBase;
        gUnk_03001548 = key;
        /* The cache payload follows its four-byte header in the heap-relative buffer. */
        anchor = gUnk_02000714 + 0x0201F400;
        heapBase = gUnk_02000714;
        LZ77UnCompWram(res, (void *)0x02020400);
        anchor -= (u32)heapBase;
        anchor += 0x1004;
        asm("" : : "r"(anchor));
        result = anchor;
    } else {
        result = (void *)0x02020404;
    }
    return result;
}
