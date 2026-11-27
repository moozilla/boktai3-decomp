#include "link/wireless/scene_pool.h"

void ActorParticles_SpawnSideBurst(struct ScenePoolActor *);
void CallbackList_ResetDispatch(struct ScenePoolActor *);
void ActorParticles_EmitOffsetBurst(struct ScenePoolActor *);
void Sound_PlaySong(u32);
void ScenePool_ReleaseActorSlot(void *, struct ScenePoolActor *, struct ScenePoolEntry *, u32);
s32 Mod(s32, s32);

u32 ScenePool_StatePlayRemovalEffects(void *manager, struct ScenePoolActor *actor,
                                      struct ScenePoolEntry *entry, u32 d)
{
    u32 frame;
    if (entry->stateEntered) {
        entry->stateEntered = 0;
        ActorParticles_SpawnSideBurst(actor);
        CallbackList_ResetDispatch(actor);
        actor->forwardSpeed = 0;
        actor->lateralSpeed = 0;
        actor->forwardBias = 0;
        actor->unknownc8 = 0;
    }
    ScenePoolActor_SetStateFlags(actor, 2);
    ScenePoolActor_SetStateFlags(actor, 1);
    ScenePoolActor_SetStateFlags(actor, 0xc);
    frame = entry->stateFrame;
    if (frame <= 0x1d) {
        if (Mod(frame, 0xc) == 0) {
            ActorParticles_EmitOffsetBurst(actor);
            Sound_PlaySong(0x16e);
        }
    } else if (frame <= 0x3b) {
        if (Mod(frame, 6) == 0) {
            ActorParticles_EmitOffsetBurst(actor);
            Sound_PlaySong(0x16e);
        }
    }
    if (entry->stateFrame > 0x77)
        ScenePool_ReleaseActorSlot(manager, actor, entry, d);
    entry->stateFrame++;
    return 0;
}
