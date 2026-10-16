#include "scene_actor_attachment.h"
/* Offsets are selected by effect kind; interpretations remain tentative. */
void SceneActorAttachment_SelectCategory4Offsets(u8 kind, s32 *yOffset, s32 *zOffset)
{
    switch (kind) {
        case 0:
            *yOffset = 400;
            *zOffset = 320;
            break;
        case 1:
            *yOffset = 0;
            *zOffset = 340;
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
