#include "global.h"
struct Object {
    u8 pad0[0x5c];
    u8 motion[0x196];
    u8 registered;
};
int TerrainOverlay_QueueAddActor(void *);
void TerrainOverlay_QueueRemoveActor(void *);
int Actor_RegisterMotion(struct Object *p)
{
    int result;
    if (TerrainOverlay_QueueAddActor(p->motion) >= 0) {
        p->registered = 1;
        result = 0;
    } else
        result = -1;
    return result;
}
