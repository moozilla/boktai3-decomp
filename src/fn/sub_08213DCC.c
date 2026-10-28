#include "global.h"
#include "render.h"
#include "ram.h"
#include "runtime.h"
struct Dma {
    vu32 src, dest, control;
};
extern u32 gUnk_0300426C, gUnk_03004264;
extern u16 gUnk_0300427C;
void Mosaic_Initialize(void);
void sub_082156D8(u32);
void Video_SetWindowBounds(u32, u32, u32, u32, u32);
void Video_SetWindowLayerMasks(u32, u32, u32, u32);
void Sprite_InitSystem(void);
void ObjShape_InitTables(void);
void Anim_ResetDefaultFlags(void);
void ScanlineEffect_Initialize(void);
void Text_LoadFontResource(void);
void SpriteOam_QueueProjectedSprites(void);
void Sprite_RenderScaledList(void);
void sub_08219624(void);
void Video_SetRenderCallbacks(u32, void *, void *, void *);
void InterruptsAndVideo_OnVideoUploadTask(void);
void InterruptsAndVideo_OnGraphicsUpdateTask(void);
void Task_SetCallbacks(void *, void *, u32);
void Task_LinkToPriorityList(void *);
void InterruptsAndVideo_InitRuntime(void)
{
    u32 fill, zero = 0;
    u8 byteZero, byteOne;
    struct Dma *dma;
    u32 mask;
    struct RuntimeTask *a;
    struct RuntimeTask *b;
    fill = zero;
    dma = (struct Dma *)0x040000D4;
    dma->src = (u32)&fill;
    dma->dest = 0x06000000;
    dma->control = 0x85006000;
    (void)dma->control;
    fill = 0xa0;
    dma->src = (u32)&fill;
    dma->dest = 0x07000000;
    dma->control = 0x85000100;
    (void)dma->control;
    fill = zero;
    dma->src = (u32)&fill;
    dma->dest = 0x05000000;
    dma->control = 0x85000100;
    (void)dma->control;
    byteZero = 0;
    *(vu16 *)0x04000208 = zero;
    (void)*(vu16 *)0x04000200;
    *(vu16 *)0x04000200 = zero;
    gUnk_0300426C = 1;
    gVideoIrqPhase = zero;
    byteOne = 1;
    gUnk_0300427C = 1;
    mask = 0x2000;
    *(vu16 *)0x04000200 = mask;
    *(vu16 *)0x04000200 |= 0x1000;
    *(vu16 *)0x04000200 |= 1;
    *(vu16 *)0x04000200 |= 2;
    *(vu16 *)0x04000004 = 8;
    *(vu16 *)0x04000004 |= 0x10;
    *(vu16 *)0x04000004 |= 0x20;
    *(vu16 *)0x04000004 = (*(vu16 *)0x04000004 & 0xff) | 0x4000;
    *(vu16 *)0x04000208 = 1;
    *(vu16 *)0x04000132 = zero;
    gSpriteAttrFlags = zero;
    Mosaic_Initialize();
    sub_082156D8(0);
    Video_SetWindowBounds(0, 0, 0, 240, 160);
    Video_SetWindowBounds(1, 0, 0, 240, 160);
    Video_SetWindowLayerMasks(0x3f, 1, 0, 0);
    gQueuedObjSlotCount = zero;
    gSceneEntryActive = zero;
    Sprite_InitSystem();
    ObjShape_InitTables();
    Anim_ResetDefaultFlags();
    ScanlineEffect_Initialize();
    Text_LoadFontResource();
    Video_SetRenderCallbacks(0, SpriteOam_QueueProjectedSprites, Sprite_RenderScaledList,
                             sub_08219624);
    gSceneEffectPalettePhase = zero;
    gUnk_03004264 = zero;
    a = &gVideoUploadTask;
    Task_SetCallbacks(a, InterruptsAndVideo_OnVideoUploadTask, 0);
    b = &gGraphicsUpdateTask;
    Task_SetCallbacks(b, InterruptsAndVideo_OnGraphicsUpdateTask, 0);
    a->priority = byteZero;
    a->unk16 = byteOne;
    a->id = zero;
    b->priority = 13;
    b->unk16 = byteOne;
    b->id = zero;
    Task_LinkToPriorityList(a);
    Task_LinkToPriorityList(b);
}
