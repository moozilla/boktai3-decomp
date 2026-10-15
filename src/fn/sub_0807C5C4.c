#include "scene_actor_attachment.h"
#define SceneActorAttachment_SelectCategory10Offsets sub_0807C5C4
/* Offsets are selected by effect kind; interpretations remain tentative. */
void SceneActorAttachment_SelectCategory10Offsets(u8 kind, s32 *yOffset, s32 *zOffset)
{
    switch (kind) {
        case 0:
            *yOffset = 250;
            *zOffset = 200;
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
