#include "global.h"
#include "engine.h"
#include "ram.h"

/* tentative: adjacent-side variant of the random burst phase; the spread turns the other way */

#include "hresvelgr_travel.h"
#include "scene_vec_polar.h"

struct ReactionIds {
    u32 a, b;
};
extern const u32 gUnk_0824F734[2];
extern const u32 gUnk_0824F744[2];
s32 sub_08249350(s32, s32);
void Hresvelgr_SetAnimation(struct HresvelgrTravelActor *, void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
void DirectionBurst_Emit(const void *, u32, u32, u32);
s32 Callback_InvokeActor(void *, void *);

static inline void add(struct SceneVec *dst, struct SceneVec *a, struct SceneVec *b)
{
    dst->x = b->x + a->x;
    dst->y = b->y + a->y;
    dst->z = b->z + a->z;
}
static inline int randomValue(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)gRandomTable);
}
void Hresvelgr_UpdateAdjacentRandomBurstPhase(struct HresvelgrTravelActor *actor, s32 frame)
{
    u32 ids[2];
    u32 angles[2];
    struct SceneVec pos;
    struct HresvelgrTravelActor *self;
    *(struct ReactionIds *)ids = *(const struct ReactionIds *)gUnk_0824F734;
    *(struct ReactionIds *)angles = *(const struct ReactionIds *)gUnk_0824F744;
    self = actor;
    if (actor->changed) {
        actor->changed = 0;
        Hresvelgr_SetAnimation(actor, &actor->animation, &actor->animationData, ids[actor->side], 2,
                               6);
        actor->burstCount = sub_08249350(randomValue() >> 3, 7) + 3;
        Sound_PlaySong(0x4D2);
    }
    Hresvelgr_SetFlags(actor, 0x600040);
    if (frame == 16)
        Sound_PlaySong(0x3E2);
    if (frame > 15 && frame < actor->burstCount + 16) {
        u32 angle;
        s32 t;
        s32 count;
        s32 m = -128;
        struct SceneVec *p;
        SceneVec_SetPolarXZ(&pos, angles[actor->side], 256);
        p = &pos;
        add(p, &actor->position, p);
        p->y = actor->origin->y + 0x80;
        {
            u8 v = self->side;
            count = self->burstCount;
            angle = (v << 6) + 16;
        }
        t = frame - 16;
        if (count > 1)
            DirectionBurst_Emit(p, (angle + t * m / (count - 1)) & 255, 12, 128);
        else
            DirectionBurst_Emit(p, angle & 255, 12, 128);
    }
    if (frame >= self->burstCount + 0x2E)
        actor->finished = 1;
    Callback_InvokeActor(actor, actor->callback);
}
