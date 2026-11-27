#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/math/monster_particle_random.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

void DirectionBurst_EmitOffset(const void *, u32, u32, u32, u32, u32);
void Hresvelgr_SetAnimation(struct HresvelgrTravelActor *, void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);

s32 Callback_InvokeActor(void *, void *);
static inline void AddReactionVector(struct SceneVec *out, struct SceneVec *a, struct SceneVec *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

void Hresvelgr_UpdateRandomVolleyPhase(struct HresvelgrTravelActor *actor, s32 frame)
{
    struct SceneVec pos;
    if (actor->changed) {
        actor->changed = 0;
        Hresvelgr_SetAnimation(actor, &actor->animation, &actor->animationData, 18, 2, 4);
        gHresvelgrVolleyState = 0;
        Sound_PlaySong(0x4D2);
        actor->phaseFrame = 0;
    }
    Hresvelgr_SetFlags(actor, 0x600000);
    if (frame == 16)
        Sound_PlaySong(0x3E2);
    if (frame > 15) {
        if ((actor->phaseFrame & 3) == 0) {
            s32 a;
            s32 b;
            const u16 *table;
            frame = actor->phaseFrame >> 2;
            AddReactionVector(&pos, &actor->position, &actor->shotOffsets[actor->side]);
            pos.y = actor->origin->y + 128;
            table = gRandomTable;

            a = ((s32)ParticleRandom_ReadSample(table) >> 3) % 32 + 64;
            b = ((s32)ParticleRandom_ReadDirectSample(table) >> 3) % 64 + 96;
            DirectionBurst_EmitOffset(&pos, frame, actor->side, a, frame + 12, b);
            if (frame == 4)
                actor->finished = 1;
        }
        actor->phaseFrame++;
    }
    Callback_InvokeActor(actor, actor->callback);
}
