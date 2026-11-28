#include "global.h"
#include "system/save/delayed_script.h"
void *Task_CreateFromHeapEnd(u32 list,
                             u32 size); /* Task_Create variant (allocator Heap_AllocFromEnd) */
void Task_SetCallbacks(void *, void *, void *);
void DelayedScript_Update(struct DelayedScript *);
void DelayedScript_OnDestroy(void);
/* Start a task that runs `script` after `delay` frames (a negative delay
 * sets everyFrame), with up to `args->count` copied argument words. */
struct DelayedScript *DelayedScript_Create(u32 script, struct DelayedScriptArguments *args,
                                           s32 delay, s32 urgent)
{
    struct DelayedScript *task =
        Task_CreateFromHeapEnd(urgent ? 3 : 5, sizeof(struct DelayedScript));

    if (task) {
        if (!args) {
            task->args.count = 0;
        } else {
            u32 *source = args->values;
            u32 *dest;
            u16 i;

            task->args.values = task->values;
            dest = task->values;
            task->args.count = args->count;
            for (i = args->count; i != 0; i--)
                *dest++ = *source++;
        }
        if (delay < 0) {
            task->everyFrame = 1;
            delay = -delay;
        } else {
            task->everyFrame = 0;
        }
        task->timer = delay;
        task->script = script;
        Task_SetCallbacks(task, DelayedScript_Update, DelayedScript_OnDestroy);
    }
    return task;
}
