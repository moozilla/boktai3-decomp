#include "scene_actor_attachment.h"
#define SceneActorAttachment_SelectAlternateCategory21Offsets sub_0807C53C

void SceneActorAttachment_SelectAlternateCategory21Offsets(u8 kind, s32 *yOffset, s32 *zOffset)
{
    switch (kind) {
        case 0:
            *yOffset = 390;
            *zOffset = 310;
            break;
        case 1:
            *yOffset = 0;
            *zOffset = 320;
            break;
        case 2:
            *yOffset = 0;
            *zOffset = 260;
            break;
        case 3:
            *yOffset = 0;
            *zOffset = 260;
            break;
    }
}
