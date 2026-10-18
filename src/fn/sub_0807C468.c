#include "scene_actor_attachment.h"
/* SceneActorAttachment_SelectAlternateCommonOffsets: Selects attachment Y/Z offsets by kind; alternate common selector uses Z offset 390 for kind 3 */
#define SceneActorAttachment_SelectAlternateCommonOffsets sub_0807C468
#define SceneActorAttachment_SelectAlternateCommonOffsets sub_0807C468

void SceneActorAttachment_SelectAlternateCommonOffsets(u8 kind, s32 *yOffset, s32 *zOffset)
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
            *zOffset = 390;
            break;
    }
}
