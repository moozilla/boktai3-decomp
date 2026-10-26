#include "global.h"
#include "ram.h"
struct FieldHud {
    u8 unknown0[0xA66];
    u16 valueMode;
    void *valueSource;
};
void Hud_SetValueSource(void *source, u32 mode)
{
    struct FieldHud *hud = gFieldHud;
    if (hud) {
        hud->valueSource = source;
        hud->valueMode = mode;
    }
}
