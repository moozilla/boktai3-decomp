#include "global.h"
#include "ram.h"
u32 Script_SeekToKeyword(u32);
u32 Script_GetPc(void);
void Script_ParseStringRef(void);
u8 *Text_LookupString(void);
void Text_SetSilentCharacterFromScript(void)
{
    if (Script_SeekToKeyword(0x73)) {
        if (Script_GetPc()) {
            u8 *t;
            Script_ParseStringRef();
            t = Text_LookupString();
            gSilentGlyph = (t[1] | (t[0] << 8)) & 0xFFF;
        }
    }
}
