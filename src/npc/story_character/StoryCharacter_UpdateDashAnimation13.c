#include "npc/manager/scene_actor_lifecycle.h"
s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);
#include "scene/motion/scene_actor_terrain.h"
#include "npc/story_character/npc_tag_layout.h"
// confidence=low: state actor kind 28 dash with decreasing speed and height above terrain.
static inline s32 trunc12(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void polar(struct ContactPosition *v, u32 angle, s32 speed)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * speed);
    v->x = x;
    v->y = 0;
    v->z = trunc12(gUnk_086149C4[angle] * speed);
}
void StoryCharacter_UpdateDashAnimation13(void *ctx, struct SceneActor *npc, s32 tick)
{
    struct NpcTag *tag = (struct NpcTag *)npc->unknown20;
    u32 anim;
    if (npc->animationEntered) {
        npc->animationEntered = 0;
        npc->finished = 0;
        switch (npc->variant) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
                anim = 0;
                break;
            case 17:
                anim = 0;
                break;
            case 18:
                anim = 0;
                break;
            case 19:
                anim = 0;
                break;
            case 20:
                anim = 0;
                break;
            case 21:
                anim = 0;
                break;
            case 22:
                anim = 0;
                break;
            case 23:
                anim = 0;
                break;
            case 24:
                anim = 0;
                break;
            case 25:
                anim = 0;
                break;
            case 26:
                anim = 0;
                break;
            case 27:
                anim = 13;
                break;
            case 28:
                anim = 0;
                break;
            case 29:
                anim = 0;
                break;
            default:
                anim = 0;
                break;
        }
        Actor_PlayAnimationEntry(npc, anim, 2, 0, 4);
        tag->height = 300;
        npc->animationPhase = 0;
    }
    SceneActor_SetFlags(npc, 0x400);
    switch (npc->animationPhase) {
        case 0:
            if (npc->stateFrame > 3) {
                switch (npc->variant) {
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                    case 10:
                    case 11:
                    case 12:
                    case 13:
                    case 14:
                    case 15:
                    case 16:
                        anim = 0;
                        break;
                    case 17:
                        anim = 0;
                        break;
                    case 18:
                        anim = 0;
                        break;
                    case 19:
                        anim = 0;
                        break;
                    case 20:
                        anim = 0;
                        break;
                    case 21:
                        anim = 0;
                        break;
                    case 22:
                        anim = 0;
                        break;
                    case 23:
                        anim = 0;
                        break;
                    case 24:
                        anim = 0;
                        break;
                    case 25:
                        anim = 0;
                        break;
                    case 26:
                        anim = 0;
                        break;
                    case 27:
                        anim = 14;
                        break;
                    case 28:
                        anim = 0;
                        break;
                    case 29:
                        anim = 0;
                        break;
                    default:
                        anim = 0;
                        break;
                }
                Actor_PlayAnimationEntry(npc, anim, 0, 0, 4);
                npc->animationPhase++;
                npc->stateFrame = 0;
            }
            break;
        case 1: {
            struct ContactPosition *pos;
            polar(&npc->body.motion, npc->direction, npc->distance);
            pos = &npc->body.position;
            npc->body.position.y =
                tag->height + SceneActorTerrain_GetHeight(SceneActorTerrain_GetCellIndex(pos), pos);
            npc->distance = (u32)npc->distance * 3 >> 2;
            if (tick > 3) {
                if (tag->height >= (tick << 3))
                    tag->height -= tick << 3;
                else
                    tag->height = 0;
            }
            if ((u32)npc->distance <= 19 && tag->height == 0) {
                Sound_PlaySong(680);
                npc->animationPhase++;
                switch (npc->variant) {
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                    case 10:
                    case 11:
                    case 12:
                    case 13:
                    case 14:
                    case 15:
                    case 16:
                        anim = 0;
                        break;
                    case 17:
                        anim = 0;
                        break;
                    case 18:
                        anim = 0;
                        break;
                    case 19:
                        anim = 0;
                        break;
                    case 20:
                        anim = 0;
                        break;
                    case 21:
                        anim = 0;
                        break;
                    case 22:
                        anim = 0;
                        break;
                    case 23:
                        anim = 0;
                        break;
                    case 24:
                        anim = 0;
                        break;
                    case 25:
                        anim = 0;
                        break;
                    case 26:
                        anim = 0;
                        break;
                    case 27:
                        anim = 14;
                        break;
                    case 28:
                        anim = 0;
                        break;
                    case 29:
                        anim = 0;
                        break;
                    default:
                        anim = 0;
                        break;
                }
                Actor_PlayAnimationEntry(npc, anim, 2, 0, 4);
            }
        } break;
        case 2:
            if (DisplayAnimation_TestFinished(&npc->drawable.common))
                npc->finished = 1;
            break;
    }
}
