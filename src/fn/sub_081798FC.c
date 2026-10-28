#include "global.h"
struct RelativeGauge {
    u8 unknown0[0x140];
    s32 scale;
};
#include "scene_pool.h"
s32 Div(s32, s32);
s32 ViewportGauge_ComputeFill(struct RelativeGauge *gauge, s32 slot)
{
    struct ScenePoolPlayer *subject;
    s32 value;
    if (slot > 1 || !(subject = gParticipantActors[slot]))
        return 0;
    if (!gauge->scale)
        value = 0;
    else
        value = Div((subject->position[0] - subject->viewportOriginX) >> 12, gauge->scale);
    return Div(value * 72, 100);
}
