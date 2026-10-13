#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define TerrainOverlay_QueueCommand sub_08003BB8
struct Command {
    u16 type, id;
};
struct Manager {
    u8 task[0x28];
    u32 count;
    u16 commands[16][2];
};
struct Manager *sub_0821A2C4(u16);
s32 TerrainOverlay_QueueCommand(u32 type, u32 id)
{
    struct Manager *p = ({
        struct Manager *value = sub_0821A2C4(0x7BE3);
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
