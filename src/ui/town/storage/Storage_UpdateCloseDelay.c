#include "global.h"
#include "ui/town/storage_screen.h"
void Storage_UpdateCloseDelay(struct StorageScreen *screen)
{
    screen->timer++;
    if (screen->timer > 0x27) {
        u32 script = screen->completionScript;
        if (script)
            Script_Call(script, 0);
        Task_Kill(screen);
    }
}
