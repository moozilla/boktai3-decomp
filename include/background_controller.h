#ifndef GUARD_BACKGROUND_CONTROLLER_H
#define GUARD_BACKGROUND_CONTROLLER_H

#include "global.h"

/*
 * Background controllers (ROM 080015AC-08002AA8). Two task families share
 * one command queue layout and one blend layout. Names are provisional;
 * "guess" marks fields whose meaning is inferred from a single use.
 *
 * Background flags (p->flags): 1 use dimension offset, 2 follow camera,
 * 4 scroll mode (else move/fixed), 16 map-mode bit, 32 add camera offset,
 * 64 blink layer visibility on odd frames, 128 add drift offset (guess).
 * ScrollingBackground flags: 1 use dimension offset, 2 follow camera,
 * 8 sine-table orbit mode.
 */

/* One queued script command: up to eight s16 arguments. */
struct BackgroundCommand {
    s16 args[8];
};

/* Queue filled by Background_QueueCommand and drained by ApplyQueuedCommands. */
struct BackgroundCommandQueue {
    u32 count;
    u16 types[8];
    struct BackgroundCommand commands[8];
};

/* Normal background task (object size 0x140). */
struct Background {
    u8 task[0x18];          /* scheduler task header */
    u16 id;
    u8 flags;
    u8 layer;
    u32 resource;           /* resource id, stored as a word */
    s32 x, y;               /* position, 1/128 px */
    s32 baseX, baseY;       /* position at creation (1/128 px) */
    s32 scrollSpeedX, scrollSpeedY;     /* scroll velocity, px/frame */
    s32 scrollDistX, scrollDistY;       /* accumulated scroll, px (guess) */
    s32 moveTargetX, moveTargetY;       /* move target, 1/128 px */
    s32 moveOriginX, moveOriginY;       /* move start, 1/128 px */
    s32 cameraOffsetX, cameraOffsetY;   /* subtracted from camera, px */
    s32 driftX, driftY;                 /* added to camera with flag 128 (guess) */
    s32 limitX, limitY;     /* halfWidth/halfHeight source, times 2 */
    s32 centerX, centerY;   /* pivot for rate scaling */
    u16 moveActive, moveTime, moveDuration;
    s16 rateX, rateY;       /* parallax rate, 128 = 1:1 */
    u16 scrollTime, scrollDuration;
    u16 unk7E;              /* never touched by these functions */
    s32 scrollStartX, scrollStartY, scrollTargetX, scrollTargetY;   /* eased speed ends, 1/128 */
    u16 fadeActive;         /* 1 while fading */
    u16 fadeDuration;       /* frames remaining */
    u16 blendX, blendY;     /* current level, 8.8 fixed point */
    u16 blendStartX, blendStartY;
    u16 blendTargetX, blendTargetY;
    u16 blendStepX, blendStepY;     /* added to blendX/blendY each frame */
    u32 unkA4;              /* never touched by these functions */
    struct BackgroundCommandQueue queue;
    u32 frame;
};

/* Scrolling background task (object size 0xF8). */
struct ScrollingBackground {
    u8 task[0x18];
    u16 id;
    u8 flags;
    u8 layer;
    u16 resource;
    s16 x, y;               /* position, 1/16 px */
    s16 baseX, baseY;       /* position at creation, 1/16 px */
    s16 moveDx, moveDy;     /* per-frame step of the move command */
    s16 speedX, speedY;     /* scroll speed, 1/16 px/frame */
    u16 unk2E;              /* never touched by these functions */
    s32 accumulatedX, accumulatedY;     /* scroll accumulator, wrapped by width/height */
    s16 moveTargetX, moveTargetY;       /* 1/16 px */
    s16 offsetX, offsetY;   /* config offsets given to the map loader */
    s16 width, height;      /* wrap period, 1/16 px */
    u16 moveActive, moveTime, moveDuration;
    u16 speedScale;         /* multiplies the sine-orbit speed */
    u8 enabled;
    u8 unk4D;
    u16 fadeActive;         /* 1 while fading */
    u16 fadeDuration;       /* frames remaining */
    u16 blendX, blendY;     /* current level, 8.8 fixed point */
    u16 blendStartX, blendStartY;
    u16 blendTargetX, blendTargetY;
    u16 blendStepX, blendStepY;     /* added to blendX/blendY each frame */
    u16 unk62;
    struct BackgroundCommandQueue queue;
};

/* Script-side configuration for a normal background. */
struct BackgroundConfig {
    u8 flags, layer;
    u16 resource;
    s32 x, y;
    s32 scrollX, scrollY;
    s32 rateX, rateY;
    s32 limitX, limitY;
    s32 centerX, centerY;
    s32 offsetX, offsetY;
    s32 mode;
    s32 priority;           /* selects the task list: 0 or nonzero */
    s32 blendX, blendY;
};

/* Script-side configuration for a scrolling background. */
struct ScrollingBackgroundConfig {
    u8 flags, layer;
    u16 resource;
    s32 x, y;
    s32 offsetX, offsetY;
    s32 speedX, speedY;
    s32 scale;
    s32 width, height;
    s32 mode;
    s32 priority;           /* selects the task list: 0 or nonzero */
    s32 fade;               /* nonzero: start a fade at creation */
    s32 blendX, blendY;
};

/* Per-layer state in the hardware scroll registers (48-byte stride). */
struct BackgroundLayer {
    u16 flags;
    u8 unk02[26];
    s16 halfWidth, halfHeight;
    s16 x, y;
    u8 unk24[12];
};

/* Camera block; x/y and offsetX/offsetY are read by all controllers. */
struct Camera {
    s16 x, y;
    s16 unk4, unk6;
    s16 offsetX, offsetY;
    s16 unkC[3];
    s16 mode;
};

/* Dimensions record found through a resource reference (x/y at 8/10). */
struct BackgroundDimensions {
    s16 unk00[4];
    s16 x, y;
};

struct ResourceRef {
    u32 tag;
    struct BackgroundDimensions *data;
};

/* Map or tile resource header; maps/tiles/extra are relocated by adding the base. */
struct BackgroundResource {
    u8 signature[2];        /* "MP" for map data */
    u16 unk02;
    u16 count;              /* number of map entries */
    u16 unk06, unk08;
    u16 size;               /* tile data size used by the tile loader */
    u32 maps;
    u32 tiles;
    u32 extra;
};

struct BackgroundMapEntry {
    u32 unk00;
    u16 id;
    u16 unk06;
    u32 unk08;
};

/* Shared globals (task pointer tables are indexed by layer). */
extern struct Background *gUnk_02000000[];
extern struct ScrollingBackground *gUnk_02000010[];
extern struct BackgroundLayer gUnk_03004C30[];
extern struct Camera gUnk_030042A0;
extern s16 gUnk_03005418[2];
extern u16 gUnk_03004BD8;
extern u16 gUnk_02000550, gUnk_0200053C;
extern const s16 gUnk_086149C4[];

/* Engine functions used by this unit. */
s32 sub_0821ABA8(s32, s32);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
u8 *Script_GetPc(void);
s32 sub_0821AA00(u8 *);
u8 *sub_08216E54(u16);
struct ResourceRef *sub_0821A2C4(u16);
void *sub_0821A520(u16, u16);
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_08219574(u32, s32, s32);
void sub_0821656C(u32, s32, s32, s32, s32);
void sub_08215A74(u32, s32);
void sub_082160B8(u32, u32, void *, s32, s32, s32, s32, void *);
void sub_082161B4(s32, s32, void *, s16, s16, s32, s32 *);
void sub_082164AC(u32, void *, u16);
void sub_08215D40(u16, void *, s32, u16 *, u16);
void *Video_GetBackgroundMap(u32);
s32 Div(s32, s32);
s32 Mod(s32, s32);

/* Fixed-point helpers used by the update functions (truncating shifts). */
#define truncate7(v)                                                                               \
    ({                                                                                             \
        s32 value = (v);                                                                           \
        s32 result;                                                                                \
        if (value >= 0)                                                                            \
            result = value >> 7;                                                                   \
        else                                                                                       \
            result = -((-value) >> 7);                                                             \
        result;                                                                                    \
    })
#define truncate3(v)                                                                               \
    ({                                                                                             \
        s32 value = (v);                                                                           \
        s32 result;                                                                                \
        if (value >= 0)                                                                            \
            result = value >> 3;                                                                   \
        else                                                                                       \
            result = -((-value) >> 3);                                                             \
        result;                                                                                    \
    })
#define fixedToInt(v)                                                                              \
    ({                                                                                             \
        s32 value = (v);                                                                           \
        s32 result;                                                                                \
        if (value >= 0)                                                                            \
            result = value >> 18;                                                                  \
        else                                                                                       \
            result = -((-value) >> 18);                                                            \
        result;                                                                                    \
    })

/* Functions of this unit (ROM order). */
s32 Background_ApplyQueuedCommands(struct Background *p);
s32 Background_Update(struct Background *p);
u32 sub_08001AD0(struct Background *p);
s32 Background_Initialize(struct Background *p, u16 id, struct BackgroundConfig *c);
void *sub_08001CB0(u16 id, struct BackgroundConfig *c);
void *Background_CreateFromScript(u32 id);
s32 Background_FindMapId(struct BackgroundResource *resource, u16 id);
s32 Background_LoadMapsFromScript(void);
s32 Background_LoadMapFromScript(void);
s32 sub_080020BC(s32 layer, u16 resourceId, s32 useIndex, u16 index);
s32 Background_LoadTilesFromScript(void);
void Background_QueueCommand(u32 layer, u32 type, s32 length, s32 *args);
void sub_08002234(void);
s32 ScrollingBackground_ApplyCommands(struct ScrollingBackground *p);
s32 ScrollingBackground_Update(struct ScrollingBackground *p);
u32 sub_08002658(struct ScrollingBackground *p);
s32 ScrollingBackground_Initialize(struct ScrollingBackground *p, u16 id,
                                   struct ScrollingBackgroundConfig *c);
void *sub_08002840(u16 id, struct ScrollingBackgroundConfig *c);
void *ScrollingBackground_CreateFromScript(u32 id);
void ScrollingBackground_QueueCommand(u32 layer, u32 type, s32 length, s32 *args);
void sub_08002A58(void);

#endif
