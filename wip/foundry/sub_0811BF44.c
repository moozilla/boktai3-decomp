#define ActorParticles_EmitBurst sub_0811BF44
#define ActorParticles_Create sub_081134B0
#define gRandomIndex gUnk_03005304
#define gRandomValues gUnk_086141BC
#include "global.h"
#include "game_state.h"
#include "scene_object.h"
extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
u32 sub_081134B0(void *, struct SceneVec *, struct SceneExtent *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
static inline u8 BurstMode(void)
{
    u32 mask = 8;
    if (gUnk_02000710->flags868 & mask) return 1;
    return 0;
}
void sub_0811BF44(void *actor)
{
    struct SceneVec offset;
    struct SceneExtent velocity;
    s32 mode, i;
    Sound_PlaySong(0x90);
    Memory_Clear(&offset, 8);
    mode = BurstMode();
    for (i = 3; i >= 0; i--) {
        gUnk_03005304 = (gUnk_03005304 + 1) & 0x3ff;
        offset.x = (gUnk_086141BC[gUnk_03005304] & 31) - 7;
        gUnk_03005304 = (gUnk_03005304 + 1) & 0x3ff;
        velocity.x = (gUnk_086141BC[gUnk_03005304] & 3) - 2;
        gUnk_03005304 = (gUnk_03005304 + 1) & 0x3ff;
        velocity.y = -(gUnk_086141BC[gUnk_03005304] & 1);
        velocity.z = 0;
        {
        u16 one = 1;
        sub_081134B0(actor, &offset, &velocity, 0x409c, 0x1c22, 4, 0, mode, 0, 0xe3, 30, 0, 512, one, 0);
        sub_081134B0(actor, &offset, &velocity, 0x409c, 0x1c22, 3, 0, mode, 0, 0xe3, 12, 0, 1024, one, 0);
        }
    }
}
