#include "global.h"
void *Heap_Alloc(s32 size);
void Memory_Clear(void *dst, s32 bytes);
s32 Heap_Free(void *block); /* void in engine.h; the caller returns its r0 */
s32 sub_0822BEE4(void *);
void sub_0821B104(void);
s32 sub_0822BF60(void *);
/* Registry routine 3C8D: runs sub_0822BEE4/sub_0822BF60 on a cleared
   0x1804-byte heap buffer, then frees it; -1 on failure (tentative). */
s32 Registry3C8D_RunFromScript(void)
{
    void *p = Heap_Alloc(0x1804);
    if (p == NULL)
        return -1;
    Memory_Clear(p, 0x1804);
    if (sub_0822BEE4(p) < 0)
        return -1;
    sub_0821B104();
    if (sub_0822BF60(p) < 0)
        goto fail;
    return Heap_Free(p);
fail:
    return -1;
}
