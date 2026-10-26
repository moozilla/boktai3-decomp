#include "global.h"
#include "ram.h"

struct SceneLinkNode {
    u8 unknown0[0xC];
    struct SceneLinkNode *next;
};
struct SceneLinkList {
    u8 unknown0[0x18];
    struct SceneLinkNode *head;
};
void SceneLink_RemoveNode(struct SceneLinkNode *target)
{
    struct SceneLinkList *list = gSceneLinkList;
    if (list) {
        struct SceneLinkNode *node = list->head;
        if (node == target)
            list->head = node->next;
        else {
            struct SceneLinkNode *next;
            for (; (next = node->next) != 0;
                 node = *(struct SceneLinkNode *volatile *)&node->next) {
                if (next == target) {
                    node->next = next->next;
                    return;
                }
            }
        }
    }
}
