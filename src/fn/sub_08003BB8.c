#include "global.h"
#include "terrain_overlay.h"
// MATCHING_CONCESSION: register-pinning
struct TerrainOverlay *sub_0821A2C4(u16);
s32 TerrainOverlay_QueueCommand(u32 type, u32 id)
{
    struct TerrainOverlay *p = ({
        struct TerrainOverlay *value = sub_0821A2C4(0x7BE3);
        asm("" : "+r"(value));
        value;
    });
    if (p && p->count <= 15) {
        p->commands[p->count][0] = type;
        p->commands[p->count][1] = id;
        p->count++;
        return 0;
    }
    return -1;
}
