#include "global.h"

/* tentative: adjacent-side variant of the recoil burst phase */

#include "hresvelgr_travel.h"
#include "scene_vec_polar.h"

struct ReactionIds {
    u32 ids[2];
};
extern const u32 gUnk_0824F734[2];
void Hresvelgr_SetAnimation(struct HresvelgrTravelActor *, void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
void DirectionBurst_Emit(const void *, u32, u32, u32);
s32 Callback_InvokeActor(void *, void *);
static inline struct SceneVec *AddReactionVector(struct SceneVec *out, struct SceneVec *b,
                                                 struct SceneVec *a)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
    return out;
}

void Hresvelgr_UpdateAdjacentRecoilBurstPhase(struct HresvelgrTravelActor *actor, s32 frame)
{
    struct ReactionIds sequences;
    struct SceneVec offset;
    u32 step;
    sequences = *(const struct ReactionIds *)gUnk_0824F734;
    if (actor->changed) {
        actor->changed = 0;
        Hresvelgr_SetAnimation(actor, &actor->animation, &actor->animationData,
                               sequences.ids[actor->side], 2, 0x44);
        Sound_PlaySong(0x4D2);
    }
    {
        u32 mask = 0x600000;
        actor->flags |= mask;
    }
    if (frame == 16)
        Sound_PlaySong(0x3E2);
    step = frame - 16;
    if (step <= 4) {
        u32 angle;
        struct SceneVec *p;
        p = AddReactionVector(&offset, &actor->shotOffsets[(actor->side - 1) & 3],
                              &actor->position);
        p->y = actor->origin->y + 0x80;
        angle = ((actor->side << 6) + 16) & 255;
        {
            u32 turn = step << 4;
            angle -= 32;
            turn += angle;
            DirectionBurst_Emit(p, turn & 255, 12, 128);
        }
        SceneVec_SetPolarXZ(&actor->velocity, actor->side * 64 + 0x80, 28 - frame);
    }
    if (frame > 50)
        actor->finished = 1;
    Callback_InvokeActor(actor, actor->callback);
}
