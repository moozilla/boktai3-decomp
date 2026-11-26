#include "global.h"
/* TentacleBody_InitSegmentAnimations: Loads animation data and scripts for each tentacle segment */

s32 Div(s32, s32);
s32 AnimData_Load(void *, s32);
void Sprite_Init(void *, void *, s32);
void *ScriptProc_Start(u16, u16);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void TentacleBody_GetDirectionFrame(s32, s32 *, s32 *);

struct Anim080A8D50 {
    u8 pad0[0x8];
    u16 phase;
    u8 pad0A[0x10 - 0xA];
    void *script;
    u8 direction;
    u8 flags;
    u8 pad16[0x1a - 0x16];
    u8 delay;
    u8 pad1b[0x34 - 0x1b];
    void *owner;
};
struct Seg080A8D50 {
    u8 sprite[0x2c];
    u8 animData[0xF0 - 0x2c];
    struct Anim080A8D50 anim;
    u8 pad[0x128 - 0xF0 - 0x38];
};
struct Body080A8D50 {
    u8 pad0[0x3d4];
    struct Seg080A8D50 segs[4];
    u8 pad874[0x8da - 0x874];
    s16 count;
    u8 pad8dc[0x8ea - 0x8dc];
    u8 mode;
    u8 step;
    s32 limit;
};

void TentacleBody_InitSegmentAnimations(u8 *p, s32 animId, u32 scriptKey, u32 direction)
{
    struct Body080A8D50 *g = *(struct Body080A8D50 **)(p + 0x3d0);
    s32 first, i, phase;
    s32 row, frame;

    g->count = 4;
    g->mode = g->count;
    g->limit = 0x7fffffff;
    g->step = Div(*(s16 *)(p + 0x9c), g->count + 1);
    first = 0;
    phase = 0;
    for (i = 0; i < g->count; i++) {
        struct Seg080A8D50 *segment = &g->segs[i];
        void *animData = segment->animData;
        AnimData_Load(animData, animId);
        Sprite_Init(segment->sprite, animData, 0);
        segment->anim.direction = direction;
        segment->anim.script = ScriptProc_Start(0x922e, scriptKey);
        if (i == g->count - 1) {
            TentacleBody_GetDirectionFrame((segment->anim.direction + 0x80) & 0xff, &row, &frame);
            Anim_SetSequence(&segment->anim, segment->anim.script, 1, (u8)row, (u8)frame);
            {
                u32 t = 4;
                t |= segment->anim.flags;
                segment->anim.flags = t;
            }
        } else {
            TentacleBody_GetDirectionFrame(segment->anim.direction, &row, &frame);
            Anim_SetSequence(&segment->anim, segment->anim.script, 2, (u8)row, (u8)frame);
            segment->anim.phase = phase;
            if (++phase > 3)
                phase = 0;
        }
        segment->anim.owner = g;
        if (first == 0) {
            segment->anim.delay = i * 0x18;
            {
                u32 t = 2;
                t |= segment->anim.flags;
                segment->anim.flags = t;
            }
        } else
            first++;
    }
}
