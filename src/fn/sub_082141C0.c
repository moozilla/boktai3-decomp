#include "global.h"
#include "render.h"
#include "runtime.h"
struct Dma {
    vu32 src, dest, control;
};
extern u32 gUnk_03004260;
void Bg_UpdateScrollShadow(void);
void InterruptsAndVideo_WaitForVBlank(void);
void ScanlineEffect_StageTransfer(void);
void Palette_UploadBuffers(void);
void sub_0821537C(void);
void Text_FlushTileUploads(void);
void sub_08215964(void);
void TileAnimation_Upload(void);
void sub_08219134(void);
u32 InterruptsAndVideo_OnVideoUploadTask(void)
{
    u32 *flags;
    vu16 *disp;
    Bg_UpdateScrollShadow();
    InterruptsAndVideo_WaitForVBlank();
    ScanlineEffect_StageTransfer();
    flags = &gVideoUploadFlags;
    if (*flags & 1) {
        struct Dma *dma = (struct Dma *)0x040000d4;
        dma->src = (u32)gOamBuffer;
        dma->dest = 0x07000000;
        dma->control = 0x84000100;
        (void)dma->control;
        *flags &= ~1;
    }
    Palette_UploadBuffers();
    sub_0821537C();
    Text_FlushTileUploads();
    sub_08215964();
    TileAnimation_Upload();
    disp = (vu16 *)0x04000000;
    *disp &= 0xe0ff;
    if (!gSceneEntryActive) {
        u16 mask = gDispCntShadow;
        *disp |= mask | 0x1000;
    }
    sub_08219134();
    return gUnk_03004260 = 0;
}
