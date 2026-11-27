#include "global.h"
#include "link/wireless/scene_pool_collision.h"

int Mod(int, int);
void CharacterModel_SetAnimationFlags(void *, int);
void ActorMarkerPool_HideMarker(int);
void CallbackList_ResetDispatch(void *);
void ScenePool_ReleaseActorSlot(struct ScenePoolManager *p, struct ScenePoolActor *slot,
                                struct ScenePoolEntry *entry)
{
    u32 id = entry->index;
    int shift = Mod(id, 32);

    p->seenIds[id >> 5] &= ~(1 << shift);
    CharacterModel_SetAnimationFlags(slot->model, 1);
    ActorMarkerPool_HideMarker(slot->marker);
    ScenePool_MotionCollider(slot)->flags |= 1;
    CallbackList_ResetDispatch(slot);
    slot->entryIndex = -1;
}
