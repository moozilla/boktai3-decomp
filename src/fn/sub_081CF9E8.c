#include "terrain_cell_probe.h"
#include "contact_actor.h"
// confidence=low: animation 13 rising phase that settles onto terrain height plus 160.
struct Npc {
    u8 pad0[7];
    u8 done, changed, entered;
    u8 padA[4];
    u16 flags;
    u8 pad10[8];
    u8 phase;
    u8 pad19[0x68 - 0x19];
    struct ContactPosition pos, vel;
    u8 pad78[0x2E4 - 0x78];
    u8 (*callback)(struct Npc *);
};
#define SceneNpc_UpdateRisingToTerrain sub_081CF9E8
void SceneNpc_UpdateRisingToTerrain(void *ctx, struct Npc *npc, s32 tick)
{
    if (npc->entered) {
        s32 zero = 0;
        npc->entered = zero;
        npc->done = zero;
        Actor_PlayAnimationEntry(npc, 13, 2, 0, 4);
        npc->phase = zero;
    }
    {
        u32 mask = 0x409;
        npc->flags |= mask;
    }
    if (npc->callback(npc) != 1) {
        switch (npc->phase) {
            case 0:
                if (tick > 15)
                    npc->phase = 1;
                break;
            case 1: {
                struct Pos16 *pos = (struct Pos16 *)&npc->pos;
                u8 *props = cellProps(cellIndexAt(pos));
                u32 type = *props >> 4;
                s32 h = (*props & 0xf) << 8;
                switch (type) {
                    case 1:
                        h -= (u8)pos->z;
                        break;
                    case 2:
                        h -= (u8)pos->x;
                        break;
                }
                if (npc->pos.y >= h + 160) {
                    npc->pos.y = h + 160;
                    npc->phase++;
                } else {
                    npc->vel.y = 8;
                }
            } break;
            case 2:
                npc->done = 1;
                break;
        }
    }
}
