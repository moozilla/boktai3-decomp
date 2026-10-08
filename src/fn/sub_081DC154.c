#include "global.h"

u32 Script_ParseStringRef(u32);
u32 Text_LookupString(u32);
void Menu_DrawText(u8 *, u8 *, u32);

void sub_081DC154(u8 *a, u8 *b, u32 c, u32 d) {
    Menu_DrawText(a, b, Text_LookupString(Script_ParseStringRef(c) + d));
}
