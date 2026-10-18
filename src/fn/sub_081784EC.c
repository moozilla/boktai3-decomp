#define Hud_SetValueSource sub_081784EC
#define gFieldHud gUnk_02000214
#include "global.h"
struct FieldHud { u8 unknown0[0xA66]; u16 valueMode; void *valueSource; };
extern struct FieldHud *gUnk_02000214;
void sub_081784EC(void *source, u32 mode)
{
    struct FieldHud *hud = gUnk_02000214;
    if (hud) {
        hud->valueSource = source;
        hud->valueMode = mode;
    }
}
