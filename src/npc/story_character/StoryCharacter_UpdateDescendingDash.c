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
void StoryCharacter_UpdateDescendingDash(void *ctx, struct SceneActor *npc, s32 tick)
{
    struct NpcTag *tag = (struct NpcTag *)npc->unknown20;
    if (npc->animationEntered) {
        u32 anim;
        npc->animationEntered = 0;
        npc->finished = 0;
        switch (npc->variant) {
            case 4:
                anim = 0;
                break;
            case 5:
                anim = 0;
                break;
            case 6:
                anim = 0;
                break;
            case 7:
                anim = 0;
                break;
            case 8:
                anim = 0;
                break;
            case 9:
                anim = 0;
                break;
            case 10:
                anim = 0;
                break;
            case 11:
                anim = 0;
                break;
            case 12:
                anim = 0;
                break;
            case 13:
                anim = 0;
                break;
            case 14:
                anim = 0;
                break;
            case 15:
                anim = 0;
                break;
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
                anim = 0;
                break;
            case 28:
                anim = 7;
                break;
            case 29:
                anim = 0;
                break;
            default:
                anim = 0;
                break;
        }
        Actor_PlayAnimationEntry(npc, anim, 1, 0, 4);
        tag->height = 300;
        npc->animationPhase = 0;
    }
    SceneActor_SetFlags(npc, 0x400);
    switch (npc->animationPhase) {
        case 0:
            if (tick > 3) {
                npc->animationPhase = 1;
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
            }
        } break;
        case 2:
            npc->finished = 1;
            break;
    }
}
