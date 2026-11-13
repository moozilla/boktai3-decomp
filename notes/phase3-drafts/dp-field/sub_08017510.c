#include "global.h"
#include "render.h"
#include "engine.h"
#include "monster_particle_random.h"
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
s32 SceneTilemap_GetScrollDelta(void);
s32 Div(s32, s32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void Weather_UpdateTilemapScroll(struct Weather *p)
{
    s32 x;
    s32 y;
    s32 pixelX, pixelY, tileX, tileY;
    p->scroll += SceneTilemap_GetScrollDelta();
    x = ((s16 *)gCameraOrigin)[0] + gCamera.offsetX + p->scroll;
    y = ((s16 *)gCameraOrigin)[1] + gCamera.offsetY;
    if (!p->timer) {
        const u16 *table = gRandomTable;
        p->timer = (ParticleRandom_ReadDirectSample(table) & 127) + 128;
        if (p->alternate)
            p->target = (ParticleRandom_ReadDirectSample(table) & 3) - 1;
        else
            p->target = (ParticleRandom_ReadDirectSample(table) & 7) - 1;
    } else
        p->timer--;
    if (!(p->frame & 15)) {
        if (p->drift < p->target)
            p->drift++;
        else if (p->drift > p->target)
            p->drift--;
    }
    if (p->alternate) {
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
