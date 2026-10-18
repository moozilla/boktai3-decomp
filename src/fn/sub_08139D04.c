#include "global.h"
struct SceneProcParticle {
    u8 pad[0xca];
    u8 used;
    u8 padCB;
    u16 group;
    u8 padCE[0xa];
};
struct SceneProcParticles {
    u8 pad[0x18];
    struct SceneProcParticle entries[6];
};
void Memory_Clear(void *, u32);
void CallbackPool_MarkNextOfKind(struct SceneProcParticles *, s32, u32, u8 *);
s32 SceneProcParticles_TestCapacity(struct SceneProcParticles *p, u32 kind, s32 requested)
{
    u32 key;
    s32 groups = 0, freeCount = 0;
    u8 visited[6];
    s32 i;
    key = kind & 0xff;
    Memory_Clear(visited, 6);
    for (i = 0; i < 6; i++) {
        if (!p->entries[i].used)
            freeCount++;
        else if ((u8)p->entries[i].group == key) {
            if (!visited[i]) {
                visited[i] = 1;
                CallbackPool_MarkNextOfKind(p, i + 1, p->entries[i].group, visited);
                groups++;
            }
        }
    }
    if (groups <= 2 && freeCount >= requested)
        return 0;
    return 1;
}
