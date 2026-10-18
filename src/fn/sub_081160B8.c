#include "global.h"
void SpriteDrop_OnDeactivate(void);
void SpriteDrop_OnExpire(u8 *p)
{
    *(void (**)(void))(p + 0xb4) = SpriteDrop_OnDeactivate;
    p[0xb1] = 1;
}
