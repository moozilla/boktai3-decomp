#include "global.h"

struct Vec {
    s32 x, y, z, unk_C;
};
struct ScreenPosition {
    s16 x, y, z;
    u16 unk_06;
};
struct Sprite {
    u32 flags;
    u8 unk_04[0x18];
    struct ScreenPosition screen;
    u8 unk_24[8];
};
struct SpriteTrail {
    u8 active, phase, unk_02[2];
    struct Vec position, history[4], velocity, target;
    u32 style, argA, argB;
    s32 historyIndex;
    u32 flags;
    u8 unk_88[0x80];
    struct Sprite head[4], tail[3];
    u8 animation[0x20];
};
struct Camera {
    u8 unk_00[0x44];
    s32 x;
};
/* Read the camera pointer separately for each projected position. */
extern struct Camera *volatile gSceneCamera;
void Vector3_CopyComponents(struct Vec *, struct Vec *);

void SpriteTrail_UpdatePositions(struct SpriteTrail *p)
{
    struct Sprite *head = p->head;
    struct Sprite *tail = p->tail;
    struct Sprite *next;
    struct Vec *position;
    s32 index, i;
    u32 *flags = &p->flags;
    u32 *savedFlags;
    /* Bit 0 freezes the leading sprite and the position history. */
    if (!(*flags & 1)) {
        struct Vec *position = &p->position;
        struct ScreenPosition *out = &p->head[0].screen;
        out->x = (p->position.x >> 12) - gSceneCamera->x;
        {
            s32 y = position->y;
            s32 z = position->z;
            out->y = ((z + y) >> 12) + 96;
        }
        out->z = position->z >> 12;
    }
    index = p->historyIndex - 1;
    if (index < 0)
        index = 3;
    next = head + 1;
    savedFlags = flags;
    i = 2;
    {
        u32 offset = index * sizeof(struct Vec) + (u32) & ((struct SpriteTrail *)0)->history;
        position = (struct Vec *)(offset + (u32)p);
    }
    for (; i >= 0;) {
        struct ScreenPosition *out = &next->screen;
        next->screen.x = (position->x >> 12) - gSceneCamera->x;
        {
            s32 y = position->y;
            s32 z = position->z;
            out->y = ((z + y) >> 12) + 96;
        }
        out->z = position->z >> 12;
        position--;
        index--;
        if (index < 0) {
            position = &p->history[3];
            index = 3;
        }
        tail->screen.x = (next->screen.x + head->screen.x) >> 1;
        tail->screen.y = (next->screen.y + head->screen.y) >> 1;
        tail->screen.z = (next->screen.z + head->screen.z) >> 1;
        head++;
        tail++;
        i--;
        next++;
    }
    if (!(*savedFlags & 1)) {
        Vector3_CopyComponents(&p->history[p->historyIndex], &p->position);
        p->historyIndex++;
        if (p->historyIndex > 3)
            p->historyIndex = 0;
    }
}
