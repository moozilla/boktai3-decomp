#include "scene/actor/scene_actor_attachment.h"
void SceneActorEffect_SortCandidates(struct SceneActorEffectCandidate *entries, s32 count)
{
    s32 i;
    s32 j;

    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - 1; j++) {
            if (entries[j].key > entries[j + 1].key) {
                s32 key = entries[j].key;
                u32 value = entries[j].value;

                entries[j].key = entries[j + 1].key;
                entries[j].value = entries[j + 1].value;
                entries[j + 1].key = key;
                entries[j + 1].value = value;
            }
        }
    }
}
