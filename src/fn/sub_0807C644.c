#include "scene_actor_attachment.h"
void SceneActorAttachment_SelectOffsets(u8 category, u8 kind, s32 *yOffset, s32 *zOffset)
{
    switch (category) {
        case 4:
            SceneActorAttachment_SelectCategory4Offsets(kind, yOffset, zOffset);
            break;
        case 1:
            SceneActorAttachment_SelectCategory1Offsets(kind, yOffset, zOffset);
            break;
        case 3:
            SceneActorAttachment_SelectCategory3Offsets(kind, yOffset, zOffset);
            break;
        case 10:
            SceneActorAttachment_SelectCategory10Offsets(kind, yOffset, zOffset);
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            SceneActorAttachment_SelectCommonOffsets(kind, yOffset, zOffset);
            break;
        case 0x15:
            SceneActorAttachment_SelectCategory21Offsets(kind, yOffset, zOffset);
            break;
        case 0x17:
            SceneActorAttachment_SelectCategory23Offsets(kind, yOffset, zOffset);
            break;
        case 0x19:
            SceneActorAttachment_SelectCategory25Offsets(kind, yOffset, zOffset);
            break;
    }
}
