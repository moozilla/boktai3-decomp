#include "global.h"
#include "engine/engine_pause.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "minigames/coffin_bike/coffin_bike.h"
void SceneMessages_RegisterReceiver(void *, u32, u32);
void CoffinBike_RegisterMessageReceiver(struct CoffinBike *s)
{
    if ((Engine_PauseBits(0x800)) == 0) {
        SceneMessages_RegisterReceiver(&s->queue, s->nextScript, 0xb);
    }
}
