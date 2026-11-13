#define Memory_FillBytes sub_08219E44
#define Memory_FillWords sub_08219DB4
#include "heap_memory.h"
#include "heap.h"

void Heap_Initialize(void)
{
    gHeapHead.prev = 0;
    gHeapHead.next = 0;
    gHeapHead.size = (0x0201F400 - (u32)&gHeapHead) | 0x80000000;
}

void *Heap_Alloc(s32 bytes)
{
    struct HeapBlock *block = &gHeapHead;
    bytes += 15;
    bytes &= ~15;
    bytes += 16;
    while (block) {
        u32 flags = block->size;
        if ((flags & 0x80000000) && (s32)(flags & 0xFFFFF) >= bytes) {
            u32 remainder = (flags & 0xFFFFF) - bytes;
            if (remainder > 16) {
                struct HeapBlock *next = (struct HeapBlock *)((u8 *)block + bytes);
                next->size = remainder | 0x80000000;
                do {
                    next->prev = block;
                    next->next = block->next;
                    if (next->next)
                        next->next->prev = next;
                    block->size = bytes;
                    block->next = next;
                } while (0);
            } else
                block->size = flags & 0x7FFFFFFF;
            block->unused = 0;
            return (void *)(block + 1);
        }
        block = block->next;
    }
    return 0;
}

void *Heap_AllocFromEnd(s32 bytes)
{
    struct HeapBlock *block = &gHeapHead;
    bytes += 15;
    bytes &= ~15;
    bytes += 16;
    {
        struct HeapBlock *next;
        while ((next = block->next))
            block = next;
    }
    while (block) {
        u32 flags = block->size;

        if ((flags & 0x80000000) && (s32)(flags & 0xFFFFF) >= bytes) {
            s32 size = flags & 0xFFFFF;
            u32 remainder = size - bytes;
            if (remainder > 16) {
                struct HeapBlock *next = (struct HeapBlock *)((u32)block + size);
                next = (struct HeapBlock *)((u32)next - bytes);
                next->size = bytes;
                next->prev = block;
                next->next = block->next;
                if (next->next)
                    next->next->prev = next;
                block->size = remainder | 0x80000000;
                block->next = next;
                block = next;
            } else
                block->size = flags & 0x7FFFFFFF;
            block->unused = 0;
            return (void *)(block + 1);
        }
        block = block->prev;
    }
    return 0;
}

void Heap_Free(void *data)
{
    struct HeapBlock *block, *prev, *next;
    u32 flags;
    if (!data)
        return;
    block = (struct HeapBlock *)data - 1;
    prev = block->prev;
    next = block->next;
    flags = block->size;
    if (flags & 0x80000000)
        return;
    block->size = flags | 0x80000000;
    block->unused = 0;
    if (prev && (prev->size & 0x80000000)) {
        prev->size = ((prev->size & 0xFFFFF) + (block->size & 0xFFFFF)) | 0x80000000;
        prev->next = next;
        if (next)
            block->next->prev = prev;
        block = prev;
    }
    if (next && (next->size & 0x80000000)) {
        block->size = ((block->size & 0xFFFFF) + (next->size & 0xFFFFF)) | 0x80000000;
        block->next = next->next;
        if (block->next)
            next->next->prev = block;
    }
}

/* Memory_FillWords: Fills the requested number of 32-bit words with a supplied value using an unrolled loop */

void Memory_FillWords(u32 *p, u32 v, s32 n)
{
    while (n > 3) {
        p[0] = v;
        p[1] = v;
        p[2] = v;
        p[3] = v;
        p += 4;
        n -= 4;
    }
    while (n > 0) {
        *p++ = v;
        n--;
    }
}

void Memory_Clear(void *data, s32 bytes)
{
    u8 *ptr = data;
    s32 aligned;
    u32 zero;
    while (bytes > 0 && ((u32)ptr & 3)) {
        *ptr++ = 0;
        bytes--;
    }
    zero = 0;
    aligned = bytes & ~3;
    CpuSet(&zero, ptr, ((aligned / 4) & 0x1FFFFF) | 0x05000000);
    ptr += aligned;
    bytes &= 3;
    while (bytes > 0) {
        *ptr = 0;
        bytes--;
        ptr++;
    }
}

void Memory_FillBytes(void *dest, u32 value, s32 size)
{
    u32 fill;
    s32 words;
    u8 *dst = dest;
    value &= 0xff;
    while (size > 0 && ((u32)dst & 3)) {
        *dst++ = value;
        size--;
    }
    value = (value << 8) | value | (value << 16) | (value << 24);
    fill = value;
    words = size & ~3;
    CpuSet(&fill, dst, 0x05000000 | ((words / 4) & 0x1fffff));
    dst += words;
    size &= 3;
    while (size > 0) {
        *dst = value;
        size--;
        dst++;
    }
}

void Memory_Copy(u8 *dst, u8 *src, s32 n)
{
    while (n > 7) {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
        dst[5] = src[5];
        dst[6] = src[6];
        dst[7] = src[7];
        dst += 8;
        src += 8;
        n -= 8;
    }
    while (n > 0) {
        *dst = *src;
        src++;
        dst++;
        n--;
    }
}

// Generic 512-byte-chunk DMA copy; retains the existing linker name.

void BgGfx_LoadTiles(u8 *source, u8 *destination, u32 bytes)
{
    while (bytes > 512) {
        struct VideoDmaRegisters *dma = (struct VideoDmaRegisters *)0x040000D4;
        dma->source = (u32)source;
        dma->destination = (u32)destination;
        dma->control = 0x84000080;
        dma->control;
        source += 512;
        destination += 512;
        bytes -= 512;
    }
    if (bytes) {
        struct VideoDmaRegisters *dma = (struct VideoDmaRegisters *)0x040000D4;
        dma->source = (u32)source;
        dma->destination = (u32)destination;
        dma->control = (bytes >> 2) | 0x84000000;
        dma->control;
    }
}
