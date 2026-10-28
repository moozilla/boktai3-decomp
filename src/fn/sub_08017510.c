#include "global.h"
#include "render.h"
#include "engine.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: scrolls weather tilemap layers and adjusts their randomized drift.
#define Weather_UpdateTilemapScroll sub_08017510
struct Weather {
    u8 pad0[0x24];
    u16 scroll;
    u8 alternate;
    u8 pad27[13];
    s32 x[2], y[2];
    s32 drift, target, timer, layer;
    u8 pad54[8];
    u32 frame;
};
extern s16 gUnk_03005418[];
#define RANDOM_TABLE ((u16 *)0x0203B400)
s32 SceneTilemap_GetScrollDelta(void);
s32 Div(s32, s32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
#define Random(table, cursor, mask)                                                                \
    ({                                                                                             \
        u32 value = (gRandomIndex + 1) & 0x3ff;                                                    \
        u32 sample;                                                                                \
        gRandomIndex = value;                                                                      \
        sample = (table)[value];                                                                   \
                                                                                                   \
        sample &= mask;                                                                            \
        sample;                                                                                    \
    })
static inline u32 ReadAlternate(u8 *pointer, u8 **saved)
{
    u32 value = *pointer;
    asm("" : "+r"(value));
    *saved = pointer;
    return value;
}
void Weather_UpdateTilemapScroll(struct Weather *p)
{
    s32 x;
    s32 y;
    u8 *alternate;
    s32 pixelX, pixelY, tileX, tileY;
    p->scroll += SceneTilemap_GetScrollDelta();
    x = gUnk_03005418[0] + gCamera.offsetX + p->scroll;
    y = gUnk_03005418[1] + gCamera.offsetY;
    if (!p->timer) {
        register u16 *table asm("r3") = RANDOM_TABLE;
        register u32 *cursor asm("r4") = &gRandomIndex;
        asm("" : : "r"(table), "r"(cursor));
        p->timer = Random(table, cursor, 127) + 128;
        if (ReadAlternate(&p->alternate, &alternate))
            p->target = Random(table, cursor, 3) - 1;
        else
            p->target = Random(table, cursor, 7) - 1;
    } else {
        p->timer--;
        alternate = &p->alternate;
    }
    if (!(p->frame & 15)) {
        if (p->drift < p->target)
            p->drift++;
        else if (p->drift > p->target)
            p->drift--;
    }
    if (*alternate) {
        if (p->frame & 1) {
            p->x[0] += 2;
            p->x[0] += p->drift;
            p->y[0] -= 4;
            p->y[0] -= p->drift;
        } else {
            p->x[1] += 3;
            p->x[1] += p->drift;
            p->y[1] -= 8;
            p->y[1] -= p->drift;
        }
    } else {
        if (p->frame & 1) {
            p->x[0] += 3;
            p->x[0] += p->drift;
            p->y[0] -= 8;
            p->y[0] -= p->drift;
        } else {
            p->x[1] += 6;
            p->x[1] += p->drift;
            p->y[1] -= 16;
            p->y[1] -= p->drift;
        }
    }
    pixelX = (p->x[p->layer] + x) & 255;
    pixelY = (p->y[p->layer] + y) & 255;
    tileX = Div(x - 128, 8);
    tileY = Div(y - 128, 8) & ~1;
    BgGfx_LoadTilemapSimple(1, tileX, tileY, pixelX, pixelY);
}
