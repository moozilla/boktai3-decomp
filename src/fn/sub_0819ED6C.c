#include "global.h"
#include "hresvelgr_travel.h"

struct ReactionIds {
    u32 ids[2];
};
extern const u32 gUnk_0824F72C[2];
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
void Hresvelgr_UpdateBurstPhase(struct HresvelgrTravelActor *actor, s32 frame)
{
    struct ReactionIds sequences;
    struct SceneVec offset;
    u32 step;
    sequences = *(const struct ReactionIds *)gUnk_0824F72C;
    if (actor->changed) {
        actor->changed = 0;
        Hresvelgr_SetAnimation(actor, &actor->animation, &actor->animationData,
                               sequences.ids[actor->side], 2, 0x46);
        Sound_PlaySong(0x4D2);
    }
    Hresvelgr_SetFlags(actor, 0x600000);
    if (frame == 16)
        Sound_PlaySong(0x3E2);
    step = frame - 16;
    if (step <= 3) {
        u32 angle;
        struct SceneVec *p;
        p = AddReactionVector(&offset, &actor->shotOffsets[actor->side + 1], &actor->position);
        p->y = actor->origin->y + 0x80;
        angle = ((actor->side << 6) - 16) & 255;
        {
            u32 turn = step << 4;
            angle -= 24;
            turn += angle;
            DirectionBurst_Emit(p, turn & 255, 12, 128);
        }
    }
    if (frame > 49)
        actor->finished = 1;
    Callback_InvokeActor(actor, actor->callback);
}
