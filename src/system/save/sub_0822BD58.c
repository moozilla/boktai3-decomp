#include "global.h"
extern void *Heap_Alloc(s32 size);
extern void Heap_Free(void *p);
extern void Save_WriteBlockWithRetries(u16 block, void *data, s32 size);

bool8 sub_0822BD58(u16 block, s32 size)
{
    u16 *buf;
    s32 i;
    if (size > 0x1ff8)
        size = 0x1ff8;
    size &= ~3;
    buf = Heap_Alloc(size);
    if (buf == NULL)
        return FALSE;
    for (i = size >> 1; i > 0;) {
        u16 *p = buf;
        do
            *p++ = 0xabcd;
        while (--i != 0);
    }
    Save_WriteBlockWithRetries(block, buf, size);
    Heap_Free(buf);
    return TRUE;
}
