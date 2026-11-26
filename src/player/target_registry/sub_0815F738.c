#include "engine/shared/global.h"
#include "engine/shared/ram.h"
#include "player/target_registry/target_registry.h"

void SceneLink_RemoveNode(void *object)
{
    struct MotionTarget *target = object;
    struct MotionTargets *list = gSceneLinkList;
    if (list) {
        struct MotionTarget *node = list->first;
        if (node == target)
            list->first = node->next;
        else {
            struct MotionTarget *next;
            for (; (next = node->next) != 0; node = node->next) {
                if (next == target) {
                    node->next = next->next;
                    return;
                }
            }
        }
    }
}
