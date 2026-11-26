#include "global.h"

void Trigger_ConsumePending(u8 *, u32);

void Camera_ConsumePendingMessage(u8 *p)
{
    Trigger_ConsumePending(p + 0x20, 1);
}
