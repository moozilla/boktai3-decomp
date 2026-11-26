#include "engine/shared/global.h"
#include "engine/shared/engine.h"
#include "engine/shared/ram.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"
// confidence=low: alternates side-specific sequences and emits four directional shots per pulse.
struct Sequences {
    u32 ids[2];
};
extern const struct Sequences gUnk_0824F72C;
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void Anim_SetFrameDurationScale(void *, u32);
void Sound_PlaySong(u32);
void DirectionBurst_Emit(void *, u32, s32, s32);
s32 sub_08249350(s32, s32) __attribute__((const));
s32 Callback_InvokeActor(void *, void *);
static inline s32 NextRandom(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)table);
}
static inline void AddOffset(struct SceneVec *out, struct HresvelgrTravelActor *actor, u32 side, struct SceneVec *origin)
{
    struct SceneVec *offset = (struct SceneVec *)(((side << 3) + (u32)actor) + 0x338);
    out->x = origin->x + offset->x;
    out->y = origin->y + offset->y;
    out->z = origin->z + offset->z;
}
void Hresvelgr_UpdateAlternatingVolley(struct HresvelgrTravelActor *actor)
{
    struct Sequences sequences = gUnk_0824F72C;
    struct HresvelgrTravelActor *saved = actor;
    if (actor->changed) {
        actor->changed = 0;
        Hresvelgr_SetAnimation(actor, &actor->animation, &actor->animationData,
                               sequences.ids[actor->side], 2, 68);
        Anim_SetFrameDurationScale(&actor->animation, 8);
        gHresvelgrVolleyCount = 0;
        Sound_PlaySong(0x4d2);
    }
    Hresvelgr_SetFlags(actor, 0x600000);
    if (actor->pulse) {
        struct SceneVec pos;
        s32 i;
        AddOffset(&pos, actor, actor->side, &actor->position);
        Sound_PlaySong(0x3e2);
        for (i = 0; i < 4; i++) {
            union RandomTableView table = {gRandomTable};
            s32 random = NextRandom(table.samples);
            s32 base = saved->side << 6;
            s32 variation = (sub_08249350(random >> 3, 9) << 4) - 64;
            s32 angle, speed, duration;
            angle = (base + variation) & 255;
            speed = (NextRandom(table.samples) >> 3) % 16 + 8;
            duration = (NextRandom(table.samples) >> 3) % 64 + 96;
            DirectionBurst_Emit(&pos, angle, speed, duration);
        }
        if (gHresvelgrVolleyCount > 15) {
            actor->finished = 1;
            Anim_SetFrameDurationScale(&saved->animation, 64);
        } else {
            Hresvelgr_SetAnimation(saved, &saved->animation, &saved->animationData,
                                   sequences.ids[(saved->side + gHresvelgrVolleyCount) & 1], 2, 4);
            gHresvelgrVolleyCount++;
        }
    }
    Callback_InvokeActor(actor, actor->callback);
}
