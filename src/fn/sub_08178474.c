#include "global.h"
#include "ram.h"
struct HudGauge {
    u8 pad[0xa78];
    u16 a78, a7a, a7c;
    u8 a7e;
};
void Hud_UpdateTransientStatGauge(struct HudGauge *);

void sub_08178474(void)
{
    struct HudGauge *hud = gFieldHud;
    if (hud != NULL) {
        hud->a78 = hud->a7a = 10;
        hud->a7c = 300;
        hud->a7e = 0;
        Hud_UpdateTransientStatGauge(hud);
    }
}
