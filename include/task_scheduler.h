#ifndef GUARD_TASK_SCHEDULER_H
#define GUARD_TASK_SCHEDULER_H
#include "runtime.h"
#include "interrupts_video.h"
/* RuntimeTask is the full 0x18-byte header, including persistence at +0x16. */
void Task_LinkToPriorityList(struct RuntimeTask *task);
void Task_UnlinkFromPriorityList(struct RuntimeTask *task);
s32 Task_Kill(struct RuntimeTask *task);
struct RuntimeTask *Task_FindById(u16 id);
#endif
