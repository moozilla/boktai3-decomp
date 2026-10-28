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

extern struct BackgroundLayer gBgRecords[4];

struct ObjGfxSlot {
    u16 size;
    u16 offset;
    const void *src;
};

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

extern struct SpriteTileResource *gSpriteTileResource;

/* RAM objects; each row of symbols/ram.csv with header include/render.h. */
extern u8 *gPaletteBankData; /* palette-bank data: 32-byte banks of sixteen RGB15 colors (AnimData_SelectPaletteBank) */
extern u32 gVBlankDialogueState; /* scrolling-dialogue/video state copied during VBlank (guess) */
extern u32 gRenderListIndex; /* active list index into the three render list-head pairs */
extern u32 gSpriteRenderCallback; /* sprite render callback, called by the graphics task (function pointer) */
extern u32 gProjectedSpriteCallback; /* projected-sprite queue callback (function pointer) */
extern u32 gExtraRenderCallback; /* additional render callback (function pointer) */
extern u16 gDynamicTileSum; /* tile count of the dynamic OBJ slots, reset and accumulated each frame */
extern u16 gMainSpriteTileSum; /* tile count of the main sprites, reset and accumulated each frame */
extern u16 gPaletteBankCount; /* palette resource bank count */
extern s16 gObjTileCursor; /* OBJ tile allocation cursor (signed loads in the allocation tests) */
extern u16 gQueuedObjSlotCount; /* queued dynamic OBJ graphics slot count */
extern u16 gStaticTileCount; /* reserved/static OBJ tile count (some files read it signed) */
extern u8 *gAnimResourceSection0C; /* resource-bank base plus header offset +0xC */
extern u8 *gAnimResourceSection14; /* resource-bank base plus header offset +0x14 */
extern u32 gArmTileUploadCount; /* ARM tile-upload request count */
extern u16 gOamPriorityCounts[]; /* queued OAM entry count per priority (4) */
extern s32 gOamCount; /* direct/final OAM entry count */
extern s32 gPendingOamCount; /* depth-sorted pending OAM entry count */
extern s32 gAffineRequestCount; /* affine request count, capped at 32 */
extern u16 gBgAffineAngle; /* BG affine angle */
extern void *gBgTileUploadSrc[]; /* per-layer pending tile-upload source (4) */
extern s16 gBgAffineMatrix[4]; /* BG affine matrix coefficients */
extern u16 gDispCntShadow; /* display control / BG visibility shadow */
extern u16 gBgTileUploadParamA[]; /* per-layer tile-upload parameter (4) */
extern u16 gBgAffineMode; /* BG affine/non-affine shadow mode */
extern u16 gBgAffineMatrixBuffer[]; /* affine coefficients copied from gBgAffineMatrix, then uploaded (4) */
extern u32 gBgAffineRefX; /* BG affine reference X accumulator */
extern u16 gBgAffineScaleY; /* BG affine Y scale */
extern u32 gBgAffineRefY; /* BG affine reference Y accumulator */
extern u16 gBgTileUploadParamB[]; /* per-layer tile-upload parameter (4) */
extern u16 gBgAffineScaleX; /* BG affine X scale */
extern u32 gBgAffineRefBuffer[]; /* buffered affine X/Y reference (2) */
extern u16 gBgScrollShadow[]; /* four BG horizontal/vertical scroll pairs (8) */
extern u16 gWindowShadow0; /* window-control bound/shadow slot (which register: unknown) */
extern u16 gWindowShadow1; /* window-control bound/shadow slot */
extern u16 gWindowShadow2; /* window-control bound/shadow slot */
extern u16 gWindowShadow3; /* window-control bound/shadow slot */
extern u8 gSpriteHeightTiles[]; /* sprite height in tiles per shape/size index (16) */
extern u8 gSpriteHeightPixels[]; /* sprite height in pixels per shape/size index (16) */
extern u8 gSpriteTileArea[]; /* sprite tile count per shape/size index (16) */
extern u8 gSpriteWidthTiles[]; /* sprite width in tiles per shape/size index (16) */
extern u32 gSpriteShapeAttr[]; /* packed OAM shape/size attribute bits per index (16) */
extern u8 gSpriteWidthPixels[]; /* sprite width in pixels per shape/size index (16) */
extern s32 gBgFadeLevel; /* background fade level */
extern u32 gObjPaletteBankCount; /* allocated object-palette bank count */
extern u32 gBgFadeApplied; /* cached/applied background fade level */
extern u32 gObjFadeApplied; /* cached/applied object fade level */
extern s16 gBackdropOverride; /* backdrop override enabled flag */
extern u32 gObjPaletteReserved; /* reserved object-palette allocation count */
extern u16 gBgFadeBankMask; /* background-palette fade bank mask */
extern u32 gObjPaletteEffect; /* object-palette refresh/effect control */
extern u32 gPaletteBanksInUse; /* active/reserved palette-bank count */
extern u16 gBackdropColor; /* backdrop RGB15 color */
extern u32 gObjFadeLevel; /* object fade/effect level */
extern u32 gFadeMultiplier; /* shared fade multiplier */
extern u16 gObjFadeColor; /* object fade target color */
extern u16 *gBgPaletteSource; /* current background palette upload source */
extern u32 gGrayscaleControl; /* grayscale effect duration/control */
extern u16 gObjPaletteKeys[]; /* object palette key per bank (16) */
extern u16 gObjFadeFlags; /* object palette fade exclusion/control flags */
extern u16 gBgFadeColor; /* background fade target color */
extern u8 gMosaicByte; /* mosaic-related byte parameter (guess) */
extern u16 gMosaicHalfword; /* mosaic-related halfword parameter (guess) */
extern u32 gScanlineEffectLayer; /* scanline effect layer selector */
extern u16 *gScanlineEffectSource; /* pending 160-line effect source; consumed then cleared */
extern u32 gScanlineEffectMode; /* scanline effect mode (scroll/mosaic/brightness) */
extern u32 gSpriteAttrFlags; /* sprite render/global attribute flags */
extern u8 gSpriteMosaicAttr; /* sprite mosaic/global attribute byte */
extern u16 gMosaicParam; /* packed mosaic parameter (byte and halfword accesses) */
extern u16 gDefaultAnimFlags; /* default animation/widget flags */
extern u32 gEnginePauseFlags; /* engine pause/suspend flags (bits 1-2: sound off; task list masks) */
extern struct OamEntry gOamBuffer[]; /* final OAM entries, 8 bytes each (128) */
extern struct OamEntry gPendingOamBuffer[]; /* pending depth-sorted OAM entries (128) */
extern u16 gBgPaletteBuffer[]; /* sixteen background palette banks of 16 colors; labels inside it are interior rows */
extern u16 gObjPaletteBuffer[][16]; /* sixteen object palette banks of 16 colors */
extern u16 gBgMapCacheKey; /* Last compressed BG map resource key, compared before LZ77 unpacking (tentative cache name) */
extern u16 gSavedDispCnt; /* DISPCNT saved by Video_SaveDisplayControl */
extern u16 gSavedWindowShadow0; /* Saved window shadow slot 0 */
extern u16 gSavedWindowShadow3; /* Saved window shadow slot 3 */
extern u16 gSavedWindowShadow2; /* Saved window shadow slot 2 */
extern u16 gSavedWindowShadow1; /* Saved window shadow slot 1 */
extern u16 gSavedWinIn; /* Saved WININ register */
extern u16 gSavedWinOut; /* Saved WINOUT register */
extern u8 gTextTileUploadIndices[]; /* Queued text tile indices; bounded storage precedes its count, original guard permits index 256 */
extern u32 gTextTileUploadCount; /* Number of queued text tile DMA uploads */
extern void *gSpriteListHeads[2]; /* Two sprite render-list heads, selected by gRenderListIndex; pointees retain subsystem views */
extern void *gMainSpriteListHeads[2]; /* Two main/animation sprite-list heads; protected ARM source retains its address declaration */
extern void *gDrawableListHeads[2]; /* Two drawable/effect render-list heads, selected by gRenderListIndex */

#endif
