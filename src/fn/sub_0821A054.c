#include "global.h"
struct Task { struct Task *prev, *next; void (*update)(struct Task *), (*destroy)(struct Task *); u16 f10, flags; };
struct List { struct Task *head; u32 mask; };
extern struct List gUnk_03005280[14];
extern u32 gUnk_0300523C;
void sub_08219F94(struct Task *);
void sub_08219D38(struct Task *);
void sub_0821A054(void)
{
    struct List *list = gUnk_03005280;
    s32 i;
    for (i = 0; i <= 13; i++, list++) {
        if (!(gUnk_0300523C & list->mask)) {
            struct Task *task = list->head;
            while (task) {
                struct Task *following = task->next;
                if (!(task->flags & 1)) {
                    if (task->update)
                        task->update(task);
                } else {
                    if (task->destroy)
                        task->destroy(task);
                    sub_08219F94(task);
                    sub_08219D38(task);
                }
                task = following;
            }
        }
    }
}
