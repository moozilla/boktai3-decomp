#include "orbit_background.h"
#define OrbitBackground_Enable sub_08050878
void OrbitBackground_Enable(void)
{
    struct OrbitBackground *background = gUnk_0200048C;
    if (background && !background->enabled) {
        sub_080508F0(background);
        background->enabled = 1;
    }
}
