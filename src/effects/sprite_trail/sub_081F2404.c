#include "effects/sprite_trail/sprite_trail.h"

#include "effects/background_controllers/background_controller.h"
#include "engine/shared/tilemap_panel.h"

void Vector3_CopyComponents(struct TrailVector *, struct TrailVector *);

void SpriteTrail_UpdatePositions(struct SpriteTrail *p)
{
    struct DisplaySequenceSprite *head = p->head;
    struct DisplaySequenceSprite *tail = p->tail;
    struct DisplaySequenceSprite *next;
    struct TrailVector *position;
    s32 index, i;
    u32 *flags = &p->frame;
    u32 *savedFlags;
    /* Record and project a new leading position on even frames. */
    if (!(*flags & 1)) {
        struct TrailVector *position = &p->position;
        struct ContactPosition *out = &p->head[0].position;
        out->x = (p->position.x >> 12) - ((struct TilemapPanel *)gSceneCamera)->scroll.x;
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
    next = head;
    next++;
    savedFlags = flags;
    i = 2;
    {
        u32 offset =
            index * sizeof(struct TrailVector) + (u32) & ((struct SpriteTrail *)0)->history;
        position = (struct TrailVector *)(offset + (u32)p);
    }
    for (; i >= 0; i--, next++) {
        struct ContactPosition *out = &next->position;
        next->position.x = (position->x >> 12) - ((struct TilemapPanel *)gSceneCamera)->scroll.x;
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
    }
    if (!(*savedFlags & 1)) {
        Vector3_CopyComponents(&p->history[p->historyIndex], &p->position);
        p->historyIndex++;
        if (p->historyIndex > 3)
            p->historyIndex = 0;
    }
}
