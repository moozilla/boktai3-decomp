#include "engine/shared/global.h"
#include "engine/shared/ram.h"

s32 sub_08222BE4(void)
{
    s32 status = gSerialTransferStatus;
    if (status >= 0)
        status = 0;
    return status;
}
