#include "global.h"
#include "system/save/delayed_script.h"
u32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
u8 *Script_GetPc(void);
s32 Script_GetKeywordValue(s32, s32);
u8 *Script_DecodeOperand(u8 *, s32 *, s32 *);
struct DelayedScript *DelayedScript_Create(u32, void *, s32, s32);
/* Script command: run a script later. Keywords: t delay, P urgent, e script
 * (by reference) or p script (by id, flag 1), a up to eight argument values. */
struct DelayedScript *Script_CreateDeferredTaskFromScript(u32 owner, u32 unused, s32 repeat)
{
    s32 delay = 0;
    s32 script = 0;
    s32 urgent, flag;
    struct DelayedScript *task;
    if (Script_SeekToKeyword('t'))
        delay = Script_GetValue();
    urgent = Script_GetKeywordValue('P', 0);
    flag = 0;
    if (Script_SeekToKeyword('e')) {
        s32 type;
        s32 value;
        Script_DecodeOperand(Script_GetPc(), &type, &value);
        script = value;
    } else if (Script_SeekToKeyword('p')) {
        flag = 1;
        script = Script_GetValue();
    }
    task = DelayedScript_Create(script, 0, delay, urgent);
    if (task) {
        task->owner = owner;
        task->flags = flag;
        if (repeat) {
            flag |= DELAYED_SCRIPT_REPEAT;
            task->flags = flag;
        }
        if (Script_SeekToKeyword('a')) {
            s32 i;
            task->args.values = task->values;
            for (i = 0; i <= 7 && Script_GetPc(); i++)
                task->args.values[i] = Script_GetValue();
            task->args.count = i;
        }
    }
    return task;
}
