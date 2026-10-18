#include "global.h"
int ResultMenu_ConfigureLayers(int, int);
void ResultMenu_InitializeLayer(void);
void MessageWindow_SetMessageTable(void *);
void MessageWindow_SetBounds(int, int, int, int);
void MessageWindow_SetPalette(int);
void MessageWindow_SetDisplayMode(int);
void sub_0801BD44(void *);
void MessageWindow_ShowMessage(int);
void ScreenTransition_SendCommand(int, int, int, int, int, int, int);
void ResultMenu_SetPhase(void *, void *);
void ResultTutorial_UpdateSequence(void *);
typedef struct {
    u32 pad[2], flags;
} Graphic;
void ResultTutorial_Open(u8 *p)
{
    switch (p[0xaf]) {
        case 0:
            p[0xb7] = ResultMenu_ConfigureLayers(14, 1);
            ResultMenu_InitializeLayer();
            MessageWindow_SetMessageTable(*(void **)(p + 0x74));
            MessageWindow_SetBounds(1, 1, 30, 6);
            MessageWindow_SetPalette(0x81b);
            MessageWindow_SetDisplayMode(1);
            sub_0801BD44(p);
            MessageWindow_ShowMessage(p[0xb6]);
            ScreenTransition_SendCommand(0, 5, 4, 4, 4, 0xffff, 0);
            ++p[0xaf];
            *(u16 *)(p + 0xaa) = 0;
            break;
        case 1:
            if (++*(s16 *)(p + 0xaa) > 31) {
                ((Graphic *)(p + 0x684))->flags &= ~1;
                p[0xb5] = 16;
                ResultMenu_SetPhase(p, ResultTutorial_UpdateSequence);
            }
            break;
    }
}
