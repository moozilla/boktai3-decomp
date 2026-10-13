#ifndef GUARD_SCENE_INDICATOR_H
#define GUARD_SCENE_INDICATOR_H

#include "engine.h"

struct Vec {
    s16 x, y, z, pad;
};

/* Sprite object embedded in an indicator (0x2C bytes). */
struct IndicatorSprite {
    u32 flags; /* bit 0: hidden */
    u8 pad4[12];
    u16 frame;
    u8 pad12[10];
    struct Vec position;
    u8 tail[8];
};

/* One blinking terrain marker (0xB4 bytes). */
struct SceneIndicator {
    u8 pad0[8];
    u32 timer; /* frames in the current phase (mode 0) */
    int id;    /* script id ('n') */
    u8 active;
    s8 frame;                                            /* base sprite frame */
    u8 animation, index, count, visible, mode, disabled; /* mode: 0 timed, 1 triggered, 2 manual */
    int delay, period, value;
    u16 times[8];  /* phase durations, alternating hidden/visible */
    u8 effect[84]; /* collider */
    struct IndicatorSprite sprite;
};

static inline void IndicatorSprite_SetFrame(struct IndicatorSprite *s, int frame)
{
    s->frame = frame;
}

struct IndicatorAnimation {
    u8 pad[6];
    u16 frame;
    u8 tail[20];
};

/* Scene indicator controller task (0xBD0 bytes, priority list 8). */
struct SceneIndicators {
    u8 task[24];
    int count;
    struct IndicatorAnimation animations[4];
    struct SceneIndicator indicators[16];
    int tile; /* terrain cell index under the player */
};

struct SceneActor {
    u8 pad0[0x30];
    s16 x;
    u8 pad32[2];
    s16 y;
};

extern struct SceneIndicators *gUnk_0200004C;
extern struct SceneActor *gUnk_02000580;

struct SceneIndicators *sub_08011884(void);
void SceneIndicator_Initialize(struct SceneIndicators *m, struct SceneIndicator *p);
void SceneIndicator_Update(struct SceneIndicators *owner, struct SceneIndicator *p);
void sub_08011A18(struct SceneIndicators *owner, struct SceneIndicator *p);
int SceneIndicators_Update(struct SceneIndicators *m);
u32 sub_08011AA4(struct SceneIndicators *m);
int SceneIndicators_Initialize(struct SceneIndicators *m);
struct SceneIndicators *sub_08011C00(void);
struct SceneIndicator *sub_08011C4C(struct SceneIndicators *m);
struct SceneIndicator *sub_08011C6C(struct SceneIndicators *m, u32 key);
int SceneIndicator_CreateFromScript(void);
int SceneIndicator_Pause(void);
int SceneIndicator_Resume(void);

u8 *sub_0821D64C(int, int);
int sub_0821DA14(void *);
void sub_08214588(void *, void *);
void sub_0821FF50(void *, int, int);
void sub_0821FF58(void *, int, int, int, int, int);

#endif
