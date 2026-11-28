#include "global.h"
#include "engine/video/render.h"
#include "ui/town/storage_screen.h"
void Storage_LoadDisplayResources(struct StorageScreen *screen)
{
    u8 *resource = ScriptProc_Start(0x92B3, 0x81B);
    ((struct StorageTextView *)screen)->paletteResource = resource + 0x1b4;
    CpuSet(resource + 0x1b4, gBgPaletteBank13, 0x04000018);
    screen->choiceRepeat = 0;
}
