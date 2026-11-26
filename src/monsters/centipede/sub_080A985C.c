#include "monsters/centipede/centipede_body.h"
#include "scene/collision/scene_body_api.h"
void TerrainOverlay_QueueAddActor(void *);
inline static u8 TestMask(u8 *flags, char mask)
{
    if ((*flags) & mask)
        return 1;
    return 0;
}

void Centipede_ActivateMarkedSegment(struct RoomActor *actor)
{
    struct TentacleBody *g = actor->context;
    s32 found = 0xff;
    s32 i;
    for (i = 0; i < g->segmentCount; i++) {
        if (TestMask(&g->segments[i].animation.status, 4)) {
            found = i;
            break;
        }
    }

    if (found != 0xff) {
        u16 m;
        SceneBody_Init(&g->markedSegmentBody, actor->body.id + 1, &g->segments[found].position, 0, 7, g);
        g->markedSegmentBody.sprite = &g->segments[found];
        TerrainOverlay_QueueAddActor(&g->markedSegmentBody);
        m = 1;
        g->attackFlags |= m;
    }
}
