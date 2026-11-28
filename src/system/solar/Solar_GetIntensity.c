#include "global.h"
#include "link/session/session_save.h"
s32 Solar_GetMeasurement(void);
/* Time left before the stored limit, clamped to 140; 0 if out of range. */
s32 Solar_GetIntensity(void)
{
    s32 now = Solar_GetMeasurement();
    s32 limit;
    if (now < 0 || now > (limit = ((struct SessionSave *)gSessionState)->solarBaseline))
        return 0;
    now = limit - now;
    if (now > 139)
        return 140;
    return now;
}
