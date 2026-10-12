#include "global.h"
// Generic 512-byte-chunk DMA copy; retains the existing linker name.
struct DmaTransfer { volatile u32 source, destination, control; };
void BgGfx_LoadTiles(u8 *source, u8 *destination, u32 bytes)
{
    while (bytes > 512) {
        struct DmaTransfer *dma = (struct DmaTransfer *)0x040000D4;
        dma->source = (u32)source;
        dma->destination = (u32)destination;
        dma->control = 0x84000080;
        dma->control;
        source += 512;
        destination += 512;
        bytes -= 512;
    }
    if (bytes) {
        struct DmaTransfer *dma = (struct DmaTransfer *)0x040000D4;
        dma->source = (u32)source;
        dma->destination = (u32)destination;
        dma->control = (bytes >> 2) | 0x84000000;
        dma->control;
    }
}
