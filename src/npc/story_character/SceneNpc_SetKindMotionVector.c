#include "npc/story_character/scene_character.h"
void SceneNpc_SetKindMotionVector(struct SceneNpc *p, s32 kind)
{
    s32 height;
    struct SceneActorBody *body;
    switch (kind) {
        case 19:
        case 20:
        case 21:
            height = 240;
            break;
        case 28:
        case 43:
            height = 200;
            break;
        case 29: {
            s32 variant = p->effects.tag;
            height = 400;
            if (variant != 1)
                break;
        }
        case 9:
            height = 528;
            break;
        default:
            height = 400;
            break;
    }
    body = &p->body;
    body->positionOffset.x = 0;
    body->positionOffset.y = height;
    body->positionOffset.z = 0;
}
