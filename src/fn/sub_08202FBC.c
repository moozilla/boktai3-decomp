#define ProximitySequence_TestPlayerDistance sub_08202FBC
#include "global.h"
#include "scene_vec.h"
#include "display_animation.h"

struct ProximitySequence {
    u8 unknown00[0x3c];
    struct SceneVec position;
    u8 unknown44[0x8c];
    void (*update)(struct ProximitySequence *);
    struct DisplaySequenceState animation;
    s32 triggerDistanceSquared;
    u8 unknownE8[8];
    u16 timer;
    u8 unknownF2[0xe];
    void *resource;
};
struct SceneVec *Player_GetEffectivePosition(s32);
void Anim_RestartSequence(struct DisplaySequenceState *, void *, u32, u32, u32);
void sub_0820302C(struct ProximitySequence *);

s32 sub_08202FBC(struct ProximitySequence *s)
{
    struct SceneVec *player = Player_GetEffectivePosition(0);
    struct SceneVec *position = &s->position;
    s32 dx = player->x - s->position.x;
    s32 dz = player->z - position->z;
    if (dx * dx + dz * dz < s->triggerDistanceSquared) {
        s->update = sub_0820302C;
        Anim_RestartSequence(&s->animation, s->resource, 7, 0, 0);
        s->timer = 180;
    }
    return 0;
}
