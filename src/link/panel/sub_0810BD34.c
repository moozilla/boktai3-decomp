#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "scene/areas/area_effect_link.h"

/* The session data's random seed. */
struct LinkSeedData {
    u8 unknown0[0x60];
    u16 seed;
};

void sub_0810BD34(struct AreaEffectLinkState *p)
{
    struct LinkSeedData *data = (struct LinkSeedData *)p->sessionData;
    s32 i;

    p->menuElapsed = 0;
    for (i = 0; i < 4; i++) {
        p->actorValues[i] = 0;
        gSceneSelectionDisplaySlots[i] = 0x3ff;
    }
    p->energyConsumed = 0;
    p->sendBit = 0;
    p->receiveBit = 0;
    gRandomIndex = data->seed & 0x3ff;
}
