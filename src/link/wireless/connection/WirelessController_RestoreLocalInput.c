#include "global.h"
#include "engine/ram.h"
#include "link/wireless/rfu_manager.h"
#include "lib/rfu/rfu_compat.h"
#include "link/wireless/wireless_controller.h"
void Memory_Clear(void *, int);
struct ModeState {
    u8 pad0;
    u8 mode;
};
int WirelessController_RestoreLocalInput(struct Engine *p)
{
    int result;
    if (!(p->pendingInput & 0x4000)) {
        result = -1;
        goto done;
    }
    p->pendingInput = p->localInput;
    result = 0;
done:
    return result;
}
