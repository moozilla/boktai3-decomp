#include "global.h"
#include "engine/engine.h"
#include "player/player.h"
#include "scene/terrain/terrain_sampling.h"
/* Tests whether the terrain cell just ahead of the player is low enough to step onto (tentative). */
void Player_UpdateActionFacingSide(struct Player *);
extern const s16 gUnk_086149C4[];

static inline s32 Scale(int x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}
s32 Player_TryStepForward(struct Player *p)
{
    if (p->gateCount != 0)
        return 1;
    {
        u8 axis = p->coordinateAxis;
        u32 a = (axis + 7) & 7;
        u32 b = (axis + 9) & 7;
        s32 h = (s16)p->directionHistory[0];
        if (h == axis || h == a || h == b) {
            struct ContactPosition pt;
            u32 ang = ((p->coordinateAxis + 1) & 7) << 5;
            u32 height;
            pt = *(struct ContactPosition *)&p->position;
            pt.x += -Scale(gUnk_086149C4[(ang + 0x40) & 0xff] * 0x32);
            pt.z += -Scale(gUnk_086149C4[ang] * 0x32);
            height = Terrain_SampleHeight(&pt);
            if (height >= (u32)(p->position.y + 0xc8))
                return 1;
            p->facing = (p->coordinateAxis + 4) & 7;
            Player_UpdateActionFacingSide(p);
            p->tetherMode = 0;
        } else {
            p->facing = (axis + 4) & 7;
            Player_UpdateActionFacingSide(p);
            p->tetherMode = 0;
        }
    }
    return 0;
}
