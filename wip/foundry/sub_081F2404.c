#define SpriteTrail_UpdatePositions sub_081F2404
#define gTrailCamera gUnk_020004B8
#include "global.h"
struct TrailVector { s32 x, y, z, unused; };
struct TrailSprite { u8 unknown[0x1c]; s16 x, y, z; u8 tail[10]; };
struct SpriteTrail {
    u32 unknown0;
    struct TrailVector position;
    struct TrailVector history[4];
    u8 unknown54[0x2c];
    s32 historyIndex;
    u32 flags;
    u8 unknown88[0x80];
    struct TrailSprite sprites[7];
};
extern u8 *gUnk_020004B8;
void Vector3_CopyComponents(void *, void *);
void sub_081F2404(struct SpriteTrail *p)
{
    struct TrailSprite *previous = &p->sprites[0];
    struct TrailSprite *midpoint = &p->sprites[4];
    struct TrailSprite *current;
    struct TrailVector *v;
    s32 index, count;
    if (!(p->flags & 1)) {
        struct TrailVector *pos = &p->position;
        s16 *s = &p->sprites[0].x;
        s[0] = (pos->x >> 12) - *(s32 *)(gUnk_020004B8 + 0x44);
        s[1] = ((pos->z + pos->y) >> 12) + 0x60;
        s[2] = pos->z >> 12;
    }
    index = p->historyIndex - 1;
    if (index < 0) index = 3;
    current = &p->sprites[1];
    v = &p->history[index];
    count = 2;
    do {
        s16 *dest = &current->x;
        dest[0] = (v->x >> 12) - *(s32 *)(gUnk_020004B8 + 0x44);
        dest[1] = ((v->z + v->y) >> 12) + 0x60;
        dest[2] = v->z >> 12;
        v--;
        if (--index < 0) { v = &p->history[3]; index = 3; }
        midpoint->x = (current->x + previous->x) >> 1;
        midpoint->y = (current->y + previous->y) >> 1;
        midpoint->z = (current->z + previous->z) >> 1;
        previous++;
        midpoint++;
        current++;
    } while (--count >= 0);
    if (!(p->flags & 1)) {
        Vector3_CopyComponents(&p->history[p->historyIndex], &p->position);
        if (++p->historyIndex > 3) p->historyIndex = 0;
    }
}
