#include "scene_actor_attachment.h"
#define SceneActorAttachment_SelectCategory3Offsets sub_0807C4B4
/* Offsets are selected by effect kind; interpretations remain tentative. */
void SceneActorAttachment_SelectCategory3Offsets(u8 kind, s32 *yOffset, s32 *zOffset)
{
    switch (kind) {
        case 0:
            *yOffset = 360;
            *zOffset = 280;
            break;
        case 1:
            *yOffset = 0;
            *zOffset = 260;
            break;
        case 2:
            *yOffset = 0;
            *zOffset = 210;
            break;
        case 3:
            *yOffset = 0;
            *zOffset = 260;
            break;
    }
}
