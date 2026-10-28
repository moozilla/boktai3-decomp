#include "global.h"
#include "ram.h"
#include "runtime.h"
// Provisional name: InterruptsAndVideo_Initialize
/* Install interrupt handlers and clear video, sprite and palette memory. */
struct Dma {
    vu32 source, dest, control;
};
extern u8 gUnk_0824FFA8[], gUnk_03003A60[];
extern u32 gUnk_0300426C;
void IntrMain(void);
void InterruptsAndVideo_Initialize(void)
{
    struct Dma *dma = (struct Dma *)0x040000D4;
    u32 fill;
    dma->source = (u32)gUnk_0824FFA8;
    dma->dest = (u32)gSerialInterruptState;
    dma->control = 0x8400000D;
    dma->control;
    dma->source = (u32)IntrMain;
    dma->dest = (u32)gUnk_03003A60;
    dma->control = 0x84000200;
    dma->control;
    *(u8 **)0x03007FFC = gUnk_03003A60;
    gUnk_0300426C = 0;
    gVBlankOccurred = 0;
    fill = 0;
    dma->source = (u32)&fill;
    dma->dest = 0x06000000;
    dma->control = 0x85006000;
    dma->control;
    fill = 0xA0;
    dma->source = (u32)&fill;
    dma->dest = 0x07000000;
    dma->control = 0x85000100;
    dma->control;
    fill = 0;
    dma->source = (u32)&fill;
    dma->dest = 0x05000000;
    dma->control = 0x85000100;
    dma->control;
    *(vu16 *)0x04000200 = 0x2001;
    *(vu16 *)0x04000004 = 8;
    *(vu16 *)0x04000208 = 1;
}
