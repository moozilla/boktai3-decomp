#include "scene_actor_attachment.h"
#define SceneActorAttachment_SelectOffsets sub_0807C644
void SceneActorAttachment_SelectOffsets(u8 category, u8 kind, s32 *yOffset, s32 *zOffset)
{
    switch (category) {
        case 4:
            sub_0807C394(kind, yOffset, zOffset);
            break;
        case 1:
            sub_0807C3D8(kind, yOffset, zOffset);
            break;
        case 3:
            sub_0807C4B4(kind, yOffset, zOffset);
            break;
        case 10:
            sub_0807C5C4(kind, yOffset, zOffset);
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            sub_0807C424(kind, yOffset, zOffset);
            break;
        case 0x15:
            sub_0807C580(kind, yOffset, zOffset);
            break;
        case 0x17:
            sub_0807C604(kind, yOffset, zOffset);
            break;
        case 0x19:
            sub_0807C4F4(kind, yOffset, zOffset);
            break;
    }
}
