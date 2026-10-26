#include "global.h"
#include "ram.h"

u32 FloatingEffectManager_Create(void);

u32 FloatingEffectManager_GetOrCreate(void)
{
    u32 r = gLinkedWidgetSet;
    if (r == 0) {
        return FloatingEffectManager_Create();
    }
    return r;
}
