#include "global.h"
#include "ui/town/storage_screen.h"
void Storage_InitEntries(struct StorageScreen *);
void Storage_InitBackground(struct StorageScreen *);
void Storage_LoadDisplayResources(struct StorageScreen *);
void Storage_InitWidgets(struct StorageScreen *);
void Storage_InitTextOptions(struct StorageScreen *);
void Storage_InitWidgetPalette(struct StorageScreen *, void *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Storage_BeginModeSelection(struct StorageScreen *, s32);
void Storage_SetUpdateCallback(struct StorageScreen *, void (*)(void));
void Storage_UpdateOpeningDelay(void);
/* Screen setup: five sub-inits, Storage_InitWidgetPalette on the cursor palette, ScreenTransition_SendCommand(0, 5,
   4, 4, 4, 0xFFFF, 0), song 0x118 and update Storage_UpdateOpeningDelay. */
u32 Storage_Init(struct StorageScreen *screen)
{
    Storage_InitEntries(screen);
    Storage_InitBackground(screen);
    Storage_LoadDisplayResources(screen);
    Storage_InitWidgets(screen);
    Storage_InitTextOptions(screen);
    Storage_InitWidgetPalette(screen, screen->cursorPalette);
    ScreenTransition_SendCommand(0, 5, 4, 4, 4, 0xFFFF, 0);
    Storage_BeginModeSelection(screen, 1);
    Sound_PlaySong(0x118);
    Storage_SetUpdateCallback(screen, Storage_UpdateOpeningDelay);
    return 0;
}
