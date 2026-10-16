#include "scene_actor_attachment.h"
/* Offsets are selected by effect kind; interpretations remain tentative. */
void SceneActorAttachment_SelectCategory1Offsets(u8 kind, s32 *yOffset, s32 *zOffset)
{
    switch (kind) {
        case 0:
            *yOffset = 470;
            *zOffset = 390;
            break;
        case 1:
            *yOffset = 0;
            *zOffset = 310;
            break;
        case 2:
            *yOffset = 0;
            *zOffset = 260;
            break;
        case 3:
            *yOffset = 0;
            *zOffset = 240;
            break;
    }
}
