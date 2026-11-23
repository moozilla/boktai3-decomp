#include "global.h"
#include "ram.h"
u32 Script_SeekToKeyword(u32);
u8 *Script_GetPc(void);
u32 Script_ParseStringRef(u8 *);
u8 *Text_LookupString(u32);
void Text_SetSilentCharacterFromScript(void)
{
    if (Script_SeekToKeyword(0x73)) {
        u8 *pc = Script_GetPc();
        if (pc) {
            u8 *t;
            t = Text_LookupString(Script_ParseStringRef(pc));
            gSilentGlyph = (t[1] | (t[0] << 8)) & 0xFFF;
        }
    }
}
