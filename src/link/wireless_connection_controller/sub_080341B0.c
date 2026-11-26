#include "engine/shared/wireless_controller.h"

u32 WirelessController_SetLocalInput(WirelessController *p, u32 input)
{
    p->localInput = input;
}
