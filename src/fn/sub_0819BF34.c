#define gHresvelgrAnimationPresets gUnk_0824F700
#include "global.h"
#include "player.h"
struct Hresvelgr {
    u8 unknown0[0x10C];
    struct Player *player;
    u8 unknown110[0x64];
    u8 animationData[0x20];
    u8 animation[0x430];
    u8 palette2[0x20], palette1[0x20], palette0[0x20];
};
struct HresvelgrAnimationPresets {
    u32 sequence[3][3];
};
extern const struct HresvelgrAnimationPresets gUnk_0824F700;
void Hresvelgr_SetAnimation(struct Hresvelgr *, void *, void *, u32, u32, u32);
u32 Player_GetCollisionData(void);
void Hresvelgr_SelectPlayerAnimation(struct Hresvelgr *actor, u32 variant)
{
    struct HresvelgrAnimationPresets presets = gUnk_0824F700;
    if (variant <= 2) {
        switch (actor->player->actionMode) {
            case 0:
                Hresvelgr_SetAnimation(actor, actor->animation, actor->animationData,
                                       presets.sequence[0][variant], 2, 0);
                CpuSet((void *)Player_GetCollisionData(), actor->palette0, 0x04000008);
                break;
            case 1:
                Hresvelgr_SetAnimation(actor, actor->animation, actor->animationData,
                                       presets.sequence[1][variant], 2, 0);
                CpuSet((void *)Player_GetCollisionData(), actor->palette1, 0x04000008);
                break;
            case 2:
                Hresvelgr_SetAnimation(actor, actor->animation, actor->animationData,
                                       presets.sequence[2][variant], 2, 0);
                CpuSet((void *)Player_GetCollisionData(), actor->palette2, 0x04000008);
                break;
        }
    }
}
