#include "scene_actor_attachment.h"
void SceneActorEffect_SortCandidates(struct SceneActorEffectCandidate *entries, s32 count)
{
    /* Count is a nonnegative element count within the supplied array. */
    s32 i = 0, j, next, limit;
    count--;
    if (i < count) {
        limit = count;
        do {
            next = i + 1;
            if (count > 0) {
                struct SceneActorEffectCandidate *entry = entries;
                j = limit;
                do {
                    s32 key = entry[0].key;
                    if (key > entry[1].key) {
                        u32 value = entry[0].value;
                        entry[0].key = entry[1].key;
                        entry[0].value = entry[1].value;
                        entry[1].key = key;
                        entry[1].value = value;
                    }
                    entry++;
                    j--;
                } while (j != 0);
            }
            i = next;
            count = limit;
        } while (i < count);
    }
}
