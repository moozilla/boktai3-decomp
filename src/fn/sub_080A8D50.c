#include "global.h"
#define TentacleBody_InitSegments sub_080A8D50

struct SegmentAnim {
    u8 pad0[8];
    u16 frame;
    u8 padA[6];
    void *script;
    u8 direction, status;
    u8 pad16[4];
    u8 f1A;
    u8 pad1B[0x128 - 0x1B];
};
struct Body {
    u8 pad0[0x3d4];
    u8 sprites[4][0x128];
    u8 pad874[0x8da - 0x874];
    s16 count;
    u8 pad8dc[0x8ea - 0x8dc];
    u8 f8EA, f8EB;
    s32 f8EC;
};
#define FIELD(p, o, T) (*(T *)((u8 *)(p) + (o)))
s32 Div(s32, s32);
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void *ScriptProc_Start(u16, u16);
void TentacleBody_GetDirectionFrame(u16, s32 *, s32 *);
void Anim_SetSequence(void *, void *, u32, u8, u8);

void TentacleBody_InitSegments(u8 *p, u32 animData, u32 key, u32 direction)
{
    struct Body *g = *(struct Body **)(p + 0x3d0);
    s32 i;
    s32 phase;
    s32 first;
    s32 row, flags;
    u8 *a;
    u8 *spr;
    u8 *sprite;
    s32 off;
    g->count = 4;
    g->f8EA = 4;
    g->f8EC = 0x7fffffff;
    g->f8EB = Div(FIELD(p, 0x9c, s16), g->count + 1);
    first = 0;
    phase = 0;
    i = 0;
    if (i >= g->count)
        return;
    off = 0;
    a = (u8 *)g + 0x4c4;
    spr = (u8 *)g + 0x400;
    sprite = (u8 *)g + 0x3d4;
    do {
        struct SegmentAnim *s = (struct SegmentAnim *)a;
        AnimData_Load(spr, animData);
        Sprite_Init(sprite, spr, 0);
        s->direction = direction;
        s->script = ScriptProc_Start(0x922E, key);
        if (i == g->count - 1) {
            TentacleBody_GetDirectionFrame((s->direction + 0x80) & 0xff, &row, &flags);
            Anim_SetSequence(s, s->script, 1, row, flags);
            s->status |= 4;
        } else {
            TentacleBody_GetDirectionFrame(s->direction, &row, &flags);
            Anim_SetSequence(s, s->script, 2, row, flags);
            s->frame = phase;
            phase++;
            if (phase > 3)
                phase = 0;
        }
        FIELD(s, 0x34, u8 *) = (u8 *)g;
        if (first == 0) {
            s->f1A = off;
            s->status |= 2;
        } else {
            first++;
        }
        off += 0x18;
        a += 0x128;
        spr += 0x128;
        sprite += 0x128;
        i++;
    } while (i < g->count);
}
