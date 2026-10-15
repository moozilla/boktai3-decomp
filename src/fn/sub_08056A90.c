#include "script_utility.h"
s32 ScriptUtility_CallRepeatedFromScript(void)
{
    struct ScriptUtilityArguments args;
    s32 count = Script_GetKeywordValue('a', 0);
    s32 repeats = Script_GetKeywordValue('r', 0);
    s32 values[count];
    args.count = count;
    args.values = values;
    if (Script_SeekToKeyword('e')) {
        u32 script = Script_GetValue();
        if (Script_SeekToKeyword('d')) {
            u8 *pc = Script_GetPc();
            s32 current = 0;
            if (current < repeats)
                do {
                    s32 next;
                    s32 *destination;
                    current++;
                    next = current;
                    if (count > 0) {
                        destination = values;
                        current = count;
                        do {
                            *destination++ = sub_0821AA00(pc);
                            pc = Script_GetPc();
                        } while (--current != 0);
                    }
                    sub_0821AFB4(script, &args);
                    current = next;
                } while (current < repeats);
        } else {
            if (repeats > 0) {
                s32 i = repeats;
                do {
                    sub_0821AFB4(script, &args);
                } while (--i != 0);
            }
        }
    }
    return 0;
}
