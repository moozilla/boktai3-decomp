#include "global.h"

#define gSceneLinkList gUnk_02000598
struct SceneLinkNode {
    u8 unknown0[0xC];
    struct SceneLinkNode *next;
};
struct SceneLinkList {
    u8 unknown0[0x18];
    struct SceneLinkNode *head;
};
extern struct SceneLinkList *gUnk_02000598;
void SceneLink_RemoveNode(struct SceneLinkNode *target)
{
    struct SceneLinkList *list = gUnk_02000598;
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
