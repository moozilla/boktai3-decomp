#include "global.h"

#ifdef NONMATCHING
// fuzzy: 92.3%
// foundry draft (worker best 92.3%); what differs is mostly register allocation and instruction order
#define SceneLink_RemoveNode sub_0815F738
#define gSceneLinkList       gUnk_02000598
struct SceneLinkNode {
    u8 unknown0[0xC];
    struct SceneLinkNode *next;
};
struct SceneLinkList {
    u8 unknown0[0x18];
    struct SceneLinkNode *head;
};
extern struct SceneLinkList *gUnk_02000598;
void sub_0815F738(struct SceneLinkNode *target)
{
    struct SceneLinkList *list = gUnk_02000598;
    if (list) {
        struct SceneLinkNode *node = list->head;
        if (node == target)
            list->head = node->next;
        else {
            for (; node->next != 0; node = node->next) {
                struct SceneLinkNode *next = node->next;
                if (next == target) {
                    node->next = next->next;
                    return;
                }
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0815F738);
#endif
