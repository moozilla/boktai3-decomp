#include "sprite_trail.h"

struct Camera {
    u8 unk_00[0x44];
    s32 x;
};
/* Read the camera pointer separately for each projected position. */
extern struct Camera *volatile gSceneCamera;
void Vector3_CopyComponents(struct TrailVector *, struct TrailVector *);

void SpriteTrail_UpdatePositions(struct SpriteTrail *p)
{
    struct ContactSwarmSprite *head = p->head;
    struct ContactSwarmSprite *tail = p->tail;
    struct ContactSwarmSprite *next;
    struct TrailVector *position;
    s32 index, i;
    u32 *flags = &p->frame;
    u32 *savedFlags;
    /* Bit 0 freezes the leading sprite and the position history. */
    if (!(*flags & 1)) {
        struct TrailVector *position = &p->position;
        struct ContactPosition *out = &p->head[0].position;
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
        u32 offset =
            index * sizeof(struct TrailVector) + (u32) & ((struct SpriteTrail *)0)->history;
        position = (struct TrailVector *)(offset + (u32)p);
    }
    for (; i >= 0;) {
        struct ContactPosition *out = &next->position;
        next->position.x = (position->x >> 12) - gSceneCamera->x;
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
        tail->position.x = (next->position.x + head->position.x) >> 1;
        tail->position.y = (next->position.y + head->position.y) >> 1;
        tail->position.z = (next->position.z + head->position.z) >> 1;
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
