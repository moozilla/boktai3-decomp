#include "global.h"
struct Object {
    u8 pad0[0x5c];
    u8 motion[0x196];
    u8 registered;
};
int TerrainOverlay_QueueAddActor(void *);
void TerrainOverlay_QueueRemoveActor(void *);
int Actor_UnregisterMotion(struct Object *p)
{
    int result;
    if (p->registered) {
        TerrainOverlay_QueueRemoveActor(p->motion);
        p->registered = 0;
        result = 0;
    } else
        result = -1;
    return result;
}
