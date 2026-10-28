#ifndef GUARD_RENDER_H
#define GUARD_RENDER_H

#include "global.h"

struct OamEntry {
    u32 attributes;
    u16 tile, depth;
};

/* Inline render camera at 030042A0, distinct from the active camera task.
 * The position's last halfword is carried by aggregate copies; meaning unknown. */
struct CameraPosition {
    s16 x, y, z;
    u8 unk06[2];
};

struct CameraRenderState {
    struct CameraPosition position;
    s16 offsetX, offsetY;
    u16 unk0C; /* initialized to 1 */
    u8 unk0E[2];
    u16 unk10; /* initialized to 0 */
    s16 mode;
};

/* 0x030042A0 */
extern struct CameraRenderState gCamera;

/* Four 0x30-byte BG records; scaleX/Y also hold overlay tile coordinates.
 * flags and halfWidth/halfHeight have mode-dependent meanings (tentative). */
struct BackgroundLayer {
    u16 flags, layer;
    u8 unk04[0xC];
    u16 scaleX, scaleY;
    s16 originX, originY;
    s16 width, height;
    s16 halfWidth, halfHeight;
    s16 x, y;
    u16 *extraData, *mapData, *map;
};

/* 0x03004C30 */
extern struct BackgroundLayer gBgRecords[4];

struct ObjGfxSlot {
    u16 size;
    u16 offset;
    const void *src;
};

/* 0x03001148 */
extern struct ObjGfxSlot gObjGfxSlots[128];

struct SpriteTileEntry {
    u16 id;
    u8 unk02[6];
};

struct SpriteTileResource {
    u32 unk00;
    u16 tileCount, entryCount;
    u32 dataOffset;
    struct SpriteTileEntry entries[1];
};

/* 0x030042EC */
extern struct SpriteTileResource *gSpriteTileResource;

struct AnimResourceEntry {
    u16 id;
    u8 field2, unk03;
    u8 field4, field5, field6, field7;
    u32 offset;
};

struct AnimResourceHeader {
    u32 unk00;
    s32 count;
    u8 unk08[4];
    u32 offsetC, offset10, offset14;
    struct AnimResourceEntry entries[1];
};

/* 0x03004300 */
extern struct AnimResourceHeader *gAnimationResource;

struct SpriteAffineParam {
    u8 angle;
    s8 scaleX, scaleY;
    u8 unk03;
};

/* 0x03004310 */
extern struct SpriteAffineParam gSpriteAffineParams[32];

/* RAM objects; each row of symbols/ram.csv with header include/render.h. */
/* 0x030042E4 */
extern u8 *gPaletteBankData; /* palette-bank data: 32-byte banks of sixteen RGB15 colors (AnimData_SelectPaletteBank) */
/* 0x03004280 */
extern u32 gVBlankDialogueState; /* scrolling-dialogue/video state copied during VBlank (guess) */
/* 0x03004290 */
extern u32 gRenderListIndex; /* active list index into the three render list-head pairs */
/* 0x03004294 */
extern u32 gSpriteRenderCallback; /* sprite render callback, called by the graphics task (function pointer) */
/* 0x030042B4 */
extern u32 gProjectedSpriteCallback; /* projected-sprite queue callback (function pointer) */
/* 0x030042B8 */
extern u32 gExtraRenderCallback; /* additional render callback (function pointer) */
/* 0x030042D8 */
extern u16 gDynamicTileSum; /* tile count of the dynamic OBJ slots, reset and accumulated each frame */
/* 0x030042DC */
extern u16 gMainSpriteTileSum; /* tile count of the main sprites, reset and accumulated each frame */
/* 0x030042E0 */
extern u16 gPaletteBankCount; /* palette resource bank count */
/* 0x030042E8 */
extern s16 gObjTileCursor; /* OBJ tile allocation cursor (signed loads in the allocation tests) */
/* 0x030042F0 */
extern u16 gQueuedObjSlotCount; /* queued dynamic OBJ graphics slot count */
/* 0x030042F4 */
extern u16 gStaticTileCount; /* reserved/static OBJ tile count (some files read it signed) */
/* 0x030042F8 */
extern u8 *gAnimResourceSection0C; /* resource-bank base plus header offset +0xC */
/* 0x030042FC */
extern u8 *gAnimResourceSection14; /* resource-bank base plus header offset +0x14 */
/* 0x03004304 */
extern u32 gArmTileUploadCount; /* ARM tile-upload request count */
/* 0x03004790 */
extern u16 gOamPriorityCounts[]; /* queued OAM entry count per priority (4) */
/* 0x03004798 */
extern s32 gOamCount; /* direct/final OAM entry count */
/* 0x0300479C */
extern s32 gPendingOamCount; /* depth-sorted pending OAM entry count */
/* 0x03004BA0 */
extern s32 gAffineRequestCount; /* affine request count, capped at 32 */
/* 0x03004BB0 */
extern u16 gBgAffineAngle; /* BG affine angle */
/* 0x03004BC0 */
extern void *gBgTileUploadSrc[]; /* per-layer pending tile-upload source (4) */
/* 0x03004BD0 */
extern s16 gBgAffineMatrix[4]; /* BG affine matrix coefficients */
/* 0x03004BD8 */
extern u16 gDispCntShadow; /* display control / BG visibility shadow */
/* 0x03004BE0 */
extern u16 gBgTileUploadParamA[]; /* per-layer tile-upload parameter (4) */
/* 0x03004BEC */
extern u16 gBgAffineMode; /* BG affine/non-affine shadow mode */
/* 0x03004BF0 */
extern u16 gBgAffineMatrixBuffer[]; /* affine coefficients copied from gBgAffineMatrix, then uploaded (4) */
/* 0x03004BF8 */
extern u32 gBgAffineRefX; /* BG affine reference X accumulator */
/* 0x03004BFC */
extern u16 gBgAffineScaleY; /* BG affine Y scale */
/* 0x03004C00 */
extern u32 gBgAffineRefY; /* BG affine reference Y accumulator */
/* 0x03004C08 */
extern u16 gBgTileUploadParamB[]; /* per-layer tile-upload parameter (4) */
/* 0x03004C14 */
extern u16 gBgAffineScaleX; /* BG affine X scale */
/* 0x03004C18 */
extern u32 gBgAffineRefBuffer[]; /* buffered affine X/Y reference (2) */
/* 0x03004C20 */
extern u16 gBgScrollShadow[]; /* four BG horizontal/vertical scroll pairs (8) */
/* 0x03004CF0 */
extern u16 gWindowShadow0; /* window-control bound/shadow slot (which register: unknown) */
/* 0x03004CF8 */
extern u16 gWindowShadow1; /* window-control bound/shadow slot */
/* 0x03004CFC */
extern u16 gWindowShadow2; /* window-control bound/shadow slot */
/* 0x03004D08 */
extern u16 gWindowShadow3; /* window-control bound/shadow slot */
/* 0x03004D10 */
extern u8 gSpriteHeightTiles[]; /* sprite height in tiles per shape/size index (16) */
/* 0x03004D20 */
extern u8 gSpriteHeightPixels[]; /* sprite height in pixels per shape/size index (16) */
/* 0x03004D30 */
extern u8 gSpriteTileArea[]; /* sprite tile count per shape/size index (16) */
/* 0x03004D40 */
extern u8 gSpriteWidthTiles[]; /* sprite width in tiles per shape/size index (16) */
/* 0x03004D50 */
extern u32 gSpriteShapeAttr[]; /* packed OAM shape/size attribute bits per index (16) */
/* 0x03004D90 */
extern u8 gSpriteWidthPixels[]; /* sprite width in pixels per shape/size index (16) */
/* 0x03004DA0 */
extern s32 gBgFadeLevel; /* background fade level */
/* 0x03004DA4 */
extern u32 gObjPaletteBankCount; /* allocated object-palette bank count */
/* 0x03004DA8 */
extern u32 gBgFadeApplied; /* cached/applied background fade level */
/* 0x03004DAC */
extern u32 gObjFadeApplied; /* cached/applied object fade level */
/* 0x03004DB0 */
extern s16 gBackdropOverride; /* backdrop override enabled flag */
/* 0x030051C0 */
extern u32 gObjPaletteReserved; /* reserved object-palette allocation count */
/* 0x030051C4 */
extern u16 gBgFadeBankMask; /* background-palette fade bank mask */
/* 0x030051C8 */
extern u32 gObjPaletteEffect; /* object-palette refresh/effect control */
/* 0x030051CC */
extern u32 gPaletteBanksInUse; /* active/reserved palette-bank count */
/* 0x030051D0 */
extern u16 gBackdropColor; /* backdrop RGB15 color */
/* 0x030051D4 */
extern u32 gObjFadeLevel; /* object fade/effect level */
/* 0x030051D8 */
extern u32 gFadeMultiplier; /* shared fade multiplier */
/* 0x030051DC */
extern u16 gObjFadeColor; /* object fade target color */
/* 0x030051E0 */
extern u16 *gBgPaletteSource; /* current background palette upload source */
/* 0x030051E4 */
extern u32 gGrayscaleControl; /* grayscale effect duration/control */
/* 0x030051F0 */
extern u16 gObjPaletteKeys[]; /* object palette key per bank (16) */
/* 0x03005210 */
extern u16 gObjFadeFlags; /* object palette fade exclusion/control flags */
/* 0x03005214 */
extern u16 gBgFadeColor; /* background fade target color */
/* 0x03005218 */
extern u8 gMosaicByte; /* mosaic-related byte parameter (guess) */
/* 0x0300521C */
extern u16 gMosaicHalfword; /* mosaic-related halfword parameter (guess) */
/* 0x03005220 */
extern u32 gScanlineEffectLayer; /* scanline effect layer selector */
/* 0x03005224 */
extern u16 *gScanlineEffectSource; /* pending 160-line effect source; consumed then cleared */
/* 0x03005228 */
extern u32 gScanlineEffectMode; /* scanline effect mode (scroll/mosaic/brightness) */
/* 0x0300522C */
extern u32 gSpriteAttrFlags; /* sprite render/global attribute flags */
/* 0x03005230 */
extern u8 gSpriteMosaicAttr; /* sprite mosaic/global attribute byte */
/* 0x03005234 */
extern u16 gMosaicParam; /* packed mosaic parameter (byte and halfword accesses) */
/* 0x03005238 */
extern u16 gDefaultAnimFlags; /* default animation/widget flags */
/* 0x0300523C */
extern u32 gEnginePauseFlags; /* engine pause/suspend flags (bits 1-2: sound off; task list masks) */
/* 0x03004390 */
extern struct OamEntry gOamBuffer[]; /* final OAM entries, 8 bytes each (128) */
/* 0x030047A0 */
extern struct OamEntry gPendingOamBuffer[]; /* pending depth-sorted OAM entries (128) */
/* 0x03004FC0 */
extern u16 gBgPaletteBuffer[]; /* sixteen background palette banks of 16 colors; labels inside it are interior rows */
/* 0x03004DC0 */
extern u16 gObjPaletteBuffer[][16]; /* sixteen object palette banks of 16 colors */
/* 0x03001548 */
extern u16 gBgMapCacheKey; /* Last compressed BG map resource key, compared before LZ77 unpacking (tentative cache name) */
/* 0x0300154C */
extern u16 gSavedDispCnt; /* DISPCNT saved by Video_SaveDisplayControl */
/* 0x0300154E */
extern u16 gSavedWindowShadow0; /* Saved window shadow slot 0 */
/* 0x03001550 */
extern u16 gSavedWindowShadow3; /* Saved window shadow slot 3 */
/* 0x03001552 */
extern u16 gSavedWindowShadow2; /* Saved window shadow slot 2 */
/* 0x03001554 */
extern u16 gSavedWindowShadow1; /* Saved window shadow slot 1 */
/* 0x03001556 */
extern u16 gSavedWinIn; /* Saved WININ register */
/* 0x03001558 */
extern u16 gSavedWinOut; /* Saved WINOUT register */
/* 0x03001560 */
extern u8 gTextTileUploadIndices[]; /* Queued text tile indices; bounded storage precedes its count, original guard permits index 256 */
/* 0x03001660 */
extern u32 gTextTileUploadCount; /* Number of queued text tile DMA uploads */
/* 0x030042C0 */
extern void *gSpriteListHeads[2]; /* Two sprite render-list heads, selected by gRenderListIndex; pointees retain subsystem views */
/* 0x030042C8 */
extern void *gMainSpriteListHeads[2]; /* Two main/animation sprite-list heads; protected ARM source retains its address declaration */
/* 0x030042D0 */
extern void *gDrawableListHeads[2]; /* Two drawable/effect render-list heads, selected by gRenderListIndex */
/* 0x03004FE0 */
extern u16 gBgPaletteBank1[]; /* Interior background palette bank 1, at gBgPaletteBuffer +0x20 bytes */
/* 0x03005000 */
extern u16 gBgPaletteBank2[]; /* Interior background palette bank 2, at gBgPaletteBuffer +0x40 bytes */
/* 0x03005020 */
extern u16 gBgPaletteBank3[]; /* Interior background palette bank 3, at gBgPaletteBuffer +0x60 bytes */
/* 0x030050E0 */
extern u16 gBgPaletteBank9[]; /* Interior background palette bank 9, at gBgPaletteBuffer +0x120 bytes */
/* 0x03005140 */
extern u16 gBgPaletteBank12[]; /* Interior background palette bank 12, at gBgPaletteBuffer +0x180 bytes */
/* 0x03005164 */
extern u16 gBgPaletteBank13Color2[]; /* Interior view starting at background palette bank 13 color 2 */
/* 0x03005174 */
extern u16 gBgPaletteBank13Color10; /* Interior background palette bank 13 color 10 */
/* 0x03005176 */
extern u16 gBgPaletteBank13Color11; /* Interior background palette bank 13 color 11 */
/* 0x0300517C */
extern u16 gBgPaletteBank13Color14; /* Interior background palette bank 13 color 14 */
/* 0x03005180 */
extern u16 gBgPaletteBank14[]; /* Interior background palette bank 14, at gBgPaletteBuffer +0x1C0 bytes */
/* 0x030051A0 */
extern u16 gBgPaletteBank15[]; /* Interior background palette bank 15, at gBgPaletteBuffer +0x1E0 bytes */
/* 0x03004278 */
extern void *gScanlineDmaSource; /* HBlank DMA0 source: the staged 160-line effect buffer (VBlank handler programs DMA0 SAD) */
/* 0x03003A00 */
extern void *gScanlineDmaTarget; /* HBlank DMA0 destination: the I/O register the scanline effect writes (BGnHOFS, BGnVOFS, MOSAIC, BLDY) */
/* 0x03004270 */
extern u16 gScanlineDmaCount; /* HBlank DMA0 transfer count, 1 for the single-halfword/word-per-line transfers */
/* 0x03004274 */
extern s16 gScanlineDmaMode; /* HBlank DMA0 mode: 0 off, 1 halfword transfer, else word transfer (low confidence) */

#endif
