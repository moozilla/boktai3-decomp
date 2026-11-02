#include "sun_orb_effect.h"

void SunOrb_SetUpdateCallback(struct OrbitEffectParams *params, OrbitEffectCallback callback)
{
    params->callback = callback;
}
