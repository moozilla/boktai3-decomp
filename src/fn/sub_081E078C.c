#include "global.h"

#include "engine.h"
struct Msg {
    void *text;
    u8 *pc;
    s32 value;
};
extern struct Msg gUnk_02000378;

/* Script: text from string -c (Text_LookupString) into gUnk_02000378, with
   -d (script position) and -e (value) (tentative). */
void ScriptMessage_SetFromScript(void)
{
    u8 *pc = 0;
    if (Script_SeekToKeyword('c'))
        pc = Script_GetPc();
    gUnk_02000378.text = Text_LookupString(Script_ParseStringRef(pc));
    if (Script_SeekToKeyword('d'))
        gUnk_02000378.pc = Script_GetPc();
    if (Script_SeekToKeyword('e'))
        gUnk_02000378.value = Script_GetValue();
}
