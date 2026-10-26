#include "global.h"
#include "render.h"
struct Task {
    struct Task *prev, *next;
    void (*update)(struct Task *), (*destroy)(struct Task *);
    u16 f10, flags;
};
struct List {
    struct Task *head;
    u32 mask;
};
extern struct List gUnk_03005280[14];
void Task_UnlinkFromPriorityList(struct Task *);
void Heap_Free(struct Task *);
void TaskSystem_RunFrame(void)
{
    struct List *list = gUnk_03005280;
    s32 i;
    for (i = 0; i <= 13; i++, list++) {
        if (!(gEnginePauseFlags & list->mask)) {
            struct Task *task = list->head;
            while (task) {
                struct Task *following = task->next;
                if (!(task->flags & 1)) {
                    if (task->update)
                        task->update(task);
                } else {
                    if (task->destroy)
                        task->destroy(task);
                    Task_UnlinkFromPriorityList(task);
                    Heap_Free(task);
                }
                task = following;
            }
        }
    }
}
