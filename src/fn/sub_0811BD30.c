#include "global.h"
#include "ram.h"

/* tentative: positions sprite parts, easing two toward terrain clearance */

struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Z {
    struct H a, b;
};
struct Vec {
    s16 x, y, z;
};
struct Part {
    u8 pad[0x20];
    s16 x, y, z;
    u8 pad2[0x3A];
};
struct Sprite {
    struct Part e[5];
};
struct Actor {
    u8 pad[0x10];
    s32 x, y, z;
    u8 pad1[0x38 - 0x1C];
    s32 yOff;
    u8 pad2[0x5AC - 0x3C];
    s32 v5AC;
};
struct Cam {
    u8 pad[0x44];
    s32 x;
};
extern struct Cam *gUnk_020004B8;
extern const u16 gUnk_086141BC[];
s32 Terrain_GetCellClearance(struct Z *);
void Anim_SetFrameDurationScale(void *, u32);
void Actor_SetSpriteDepthOffsets(struct Sprite *p, s32 d);

static inline int Rand(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3FF;
    return gUnk_086141BC[gEffectRandomTableIndex];
}

void Actor_UpdateShadowSpriteParts(struct Actor *a, struct Sprite *s)
{
    struct Vec pos;
    struct Z tmp;
    s32 i;
    s32 h;
    s32 v;
    s16 *out;

    out = &pos.x;
    pos.x = (a->x >> 12) - gUnk_020004B8->x;
    out[1] = ((a->z + (a->y + a->yOff)) >> 12) + 0x5A;
    out[2] = a->z >> 12;
    for (i = 0; i < 3; i++) {
        s->e[i].x = pos.x;
        s->e[i].y = pos.y;
    }
    for (i = 3; i <= 4; i++) {
        s32 ax = a->x >> 12;
        pos.x = ax - gUnk_020004B8->x;
        tmp.a.hi = a->y >> 12;
        tmp.b.lo = a->z >> 12;
        switch (i) {
            case 3:
                pos.x += 15;
                pos.y -= 2;
                tmp.a.lo = ax + 15;
                break;
            case 4:
                pos.x -= 19;
                pos.y -= 2;
                tmp.a.lo = ax - 19;
                break;
        }
        h = Terrain_GetCellClearance(&tmp) - 6;
        if (pos.y - h > 8)
            do {
                h = pos.y - 8;
            } while (0);
        else if (pos.y - h < -8)
            do {
                h = pos.y + 8;
            } while (0);
        s->e[i].x = pos.x;
        if (s->e[i].y + 3 < h)
            s->e[i].y += 3;
        else if (s->e[i].y - 3 > h)
            s->e[i].y -= 3;
        else
            s->e[i].y = h;
        s->e[i].z = (a->z >> 12) - 6;
        if ((Rand() & 15) > 9) {
            s->e[i].y += (Rand() & 3) - 1;
        }
    }
    v = (a->v5AC >> 12) + 1;
    if (v != 1) {
        s32 sc = 0x40 - v * 2;
        sc = sc < 0x20 ? 0x20 : sc;
        Anim_SetFrameDurationScale(&s->e[3], (u16)sc);
        Anim_SetFrameDurationScale(&s->e[4], (u16)sc);
    }
    Actor_SetSpriteDepthOffsets(s, a->z >> 12);
}
