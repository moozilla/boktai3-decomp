#define SceneActor_StartRandomizedEffect sub_0813D3B0
#define gEffectRandomTable gUnk_0203B400
#define gEffectRandomIndex gUnk_03005308
#include "global.h"
#include "breakable_barrier.h"
struct SceneActorRandomEffect {
    u8 unknown0[0x5B8];
    u8 effect;
    u8 mode;
    u8 unknown5BA[6];
    struct SceneVec *position;
};

extern u32 gUnk_03005308;
void sub_082314C4(void *);
void sub_08231500(void *, s32, void *, void *, void *, s32, s32);
void sub_0813D3B0(struct SceneActorRandomEffect *p)
{
    struct BarrierExtent position, velocity, extent;
    u32 index;
    const u16 *table;
    u32 *indexPtr;
    u32 mask;
    if (p->mode == 1) {
        sub_082314C4(&p->effect);
    } else {
        if (p->mode == 2) Sound_PlaySong(308);
        else if (p->mode == 3) Sound_PlaySong(308);
        position.x = p->position->x;
        position.y = p->position->y + 128;
        position.z = p->position->z;
        table = gUnk_0203B400;
        indexPtr = &gUnk_03005308;
        index = *indexPtr;
        mask = 1023;
        index = (index + 1) & mask;
        velocity.x = (table[index] & 15) - 7;
        index = (index + 1) & mask;
        velocity.y = (table[index] & 31) - 16;
        index = (index + 1) & mask;
        *indexPtr = index;
        velocity.z = (table[index] & 15) - 7;
        extent.x = 5;
        extent.y = 10;
        extent.z = 5;
        sub_08231500(&p->effect, 3, &position, &velocity, &extent, 40, 40);
    }
}
