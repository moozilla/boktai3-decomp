#include "npc/otenko/otenko_actor.h"
#include "scene/motion/scene_actor_terrain.h"
// confidence=low: animation 9-12 dash with decreasing speed and height above terrain.
s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);
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
void Otenko_UpdateDescendingDash(void *ctx, struct OtenkoActor *npc, s32 tick)
{
    struct OtenkoEffectState *effect = &npc->effect;
    if (npc->animationEntered) {
        npc->animationEntered = 0;
        npc->done = 0;
        Actor_PlayAnimationEntry(npc, 9, 2, 0, 4);
        npc->animationPhase = 0;
        npc->body.position.y += 160;
        effect->bobOffset = 300;
    }
    {
        u32 mask = 0x409;
        npc->displayFlags |= mask;
    }
    if ((u8)npc->animationCallback(npc) != 1) {
        switch (npc->animationPhase) {
            case 0:
                if (npc->stateFrame > 3) {
                    Actor_PlayAnimationEntry(npc, 10, 2, 0, 4);
                    npc->animationPhase++;
                    npc->stateFrame = 0;
                }
                break;
            case 1: {
                struct ContactPosition *pos;
                polar(&npc->body.motion, npc->targetAngle, npc->speed);
                pos = &npc->body.position;
                npc->body.position.y =
                    effect->bobOffset +
                    SceneActorTerrain_GetHeight(SceneActorTerrain_GetCellIndex(pos), pos);
                npc->speed = ((u32)npc->speed * 3) >> 2;
                if (tick > 3) {
                    if (effect->bobOffset >= (tick << 3))
                        effect->bobOffset -= tick << 3;
                    else
                        effect->bobOffset = 0;
                }
                if ((u32)npc->speed <= 19 && effect->bobOffset == 0) {
                    Sound_PlaySong(680);
                    npc->animationPhase++;
                    Actor_PlayAnimationEntry(npc, 11, 2, 0, 4);
                }
            } break;
            case 2:
                if (DisplayAnimation_TestFinished(&npc->display)) {
                    Actor_PlayAnimationEntry(npc, 12, 0, 0, 4);
                    npc->animationPhase++;
                }
                break;
            case 3:
                npc->done = 1;
                break;
        }
    }
}
