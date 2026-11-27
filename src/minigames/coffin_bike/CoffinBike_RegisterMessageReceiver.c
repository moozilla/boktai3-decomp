#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "minigames/coffin_bike/coffin_bike.h"
void SceneMessages_RegisterReceiver(void *, u32, u32);
void CoffinBike_RegisterMessageReceiver(struct CoffinBike *s)
{
    u32 m = 0x800;
    if (((gEngineFlags | gScenePauseFlags) & m) == 0) {
        SceneMessages_RegisterReceiver(&s->queue, s->nextScript, 0xb);
    }
}
