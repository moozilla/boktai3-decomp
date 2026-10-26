#include "global.h"
#include "engine.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Body {
    u8 pad[0xc];
    struct Vec pos;
};
struct Actor {
    u8 pad0[0xa];
    u8 flashing;
    u8 padb[3];
    u16 flags;
    u8 pad10[0x4c];
    struct Body body;
    u8 pad70[0x64];
    u32 drawable;
    u8 padd8[0x1f0];
    s16 timer, duration;
};
struct Map {
    u32 pad;
    u8 *cells;
    u8 pad8[28];
    u16 rows[1];
};
u8 *TerrainModifiers_FindCellOverride(int, int);
void Obj_StepPosition(void *);
int DisplayAnimation_SetPosition(void *, void *);
int DisplayAnimation_UpdatePlayback(void *);
void Actor_SetAttachmentEffect(struct Actor *, unsigned, int, int);
static inline u8 *cell(int i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + 12 + i * 4;
}
int Actor_UpdateMovementAnimationAndEffects(void *unused, struct Actor *p)
{
    s16 k2 = 0;
    struct Body *body = &p->body;
    void *drawable = &p->drawable;
    Obj_StepPosition(body);
    {
        u32 mask = 0x400;
        if (!(p->flags & mask)) {
            struct Vec *pos = &body->pos;
            int x = pos->x >> 8, z = pos->z >> 8, index, height, slope;
            u8 *c;
            if ((((x < 0) || (z < 0)) || (((u32)x) >= gTerrainWidth)) ||
                (((u32)z) >= gTerrainHeight)) {
                index = 0;
            } else {
                index = gTerrainMap->rows[z] + x;
            }
            c = cell(index);
            slope = *c >> 4;
            height = (*c & 15) << 8;
            switch (slope) {
                case 1:
                    height -= (u8)pos->z;
                    break;
                case 2:
                    height -= (u8)pos->x;
                    break;
            }
            body->pos.y = height;
        }
    }
    DisplayAnimation_SetPosition(drawable, &body->pos);
    DisplayAnimation_UpdatePlayback(drawable);
    if (p->timer > 0) {
        p->timer--;
        if (p->timer <= 3) {
            if (p->flashing && (!(gEngineFlags & 0x200))) {
                p->timer = p->duration;
            } else {
                s16 *timer = &p->timer;
                int zero;
                zero = k2;
                *timer = 0;
                Actor_SetAttachmentEffect(p, 8, 180, 180);
                p->flashing = zero;
            }
        }
    }
}
