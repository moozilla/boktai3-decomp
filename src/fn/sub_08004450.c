#include "global.h"
void sub_0822B2F8(s32);
u8 sub_0800441C(u8 *);
void AnimatedParticles_EmitDirectional(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, void *);
void Scene_SpawnEdgeParticles(void *, s32);
void Scene_PlayMovementEffect(u8 *actor)
{
    switch (*actor) {
        case 0:
            sub_0822B2F8(0x143);
            break;
        case 1:
            sub_0822B2F8(0x143);
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            sub_0822B2F8(0x143);
            break;
        default:
            sub_0822B2F8(0x143);
            break;
    }
    if (sub_0800441C(actor))
        AnimatedParticles_EmitDirectional(4, 1, 1, 0x40, 0, 255, 2, 4, 6, 6, actor + 0xAC);
    else
        Scene_SpawnEdgeParticles(actor + 0x38, 0x90);
}
