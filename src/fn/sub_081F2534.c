#include "global.h"
#include "background_controller.h"
struct SpriteTrail {
    u8 entered, state;
    u8 pad2[2];
    struct {
        s32 x, y, z, w;
    } position;
    u8 pad14[0x70];
    u32 flags;
    u8 pad88[0x40];
    u32 spriteFlags;
};
void SpriteTrail_UpdatePositions(struct SpriteTrail *);
s32 SceneTilemap_GetScrollDelta(void);
void *SingletonSlot7_CreateImpactSprite(void *, s32, s32);
void SpriteTrail_BeginImpact(void *unused, struct SpriteTrail *p)
{
    struct {
        s16 x, y, z;
    } screen;
    s32 delta, y, z;
    s16 *out;
    u8 *pos;
    if (p->entered) {
        p->entered = 0;
        {
            u32 *flags = &p->spriteFlags;
            u32 one = 1;
            *flags |= one;
        }
    }
    SpriteTrail_UpdatePositions(p);
    delta = SceneTilemap_GetScrollDelta() >> 12;
    pos = (u8 *)&p->position;
    out = &screen.x;
    screen.x = (*(s32 *)(pos) >> 12) - *(s32 *)(((u8 *)gSceneCamera) + 0x44);
    y = *(s32 *)(pos + 4);
    z = *(s32 *)(pos + 8);
    out[1] = ((z + y) >> 12) + 0x60;
    out[2] = *(s32 *)(pos + 8) >> 12;
    SingletonSlot7_CreateImpactSprite(&screen, 6, delta);
    p->state = 2;
    p->entered = 1;
    p->flags = 1;
}
