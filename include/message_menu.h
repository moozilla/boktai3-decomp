#ifndef GUARD_MESSAGE_MENU_H
#define GUARD_MESSAGE_MENU_H

#include "engine.h"

struct MenuResource {
    u32 words[8];
};

/* Full-screen message menu task (0x4D8 bytes, priority list 11). */
struct MessageMenu {
    u8 task[24];
    u32 timer; /* frames since creation */
    u8 pad1C[6];
    u16 script;                           /* script run once when the menu is dismissed */
    u8 formatter[24];                     /* text formatter state for EncodedRecord_FormatRows */
    u8 lines[4][256];                     /* formatted text lines */
    void *source;                         /* source string table (guess) */
    u8 *text;                             /* script text for the three windows */
    int mainText, footerText, choiceText; /* text window handles */
    void *map, *palette;
    struct MenuResource resource;
    u8 transition[96]; /* animation object used for the opening effect */
};

int TextWindow_Create(int, int, int, int);
void TextWindow_SetString(int, int, void *);
void TextWindow_StartPlayback(int);
void TextWindow_Destroy(int);
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
void EncodedRecord_EncodeFields(void *);
void EncodedRecord_FormatRows(void *, void *, void *, int);
void *Script_GetTextSourcePc(void);
extern u16 gBgPaletteBuffer[];
extern u16 gBgPaletteBank15[];
extern u16 gUnk_03005260[];

void MessageMenu_LoadBackground(struct MessageMenu *m, int id);
int MessageMenu_CreateTextWindows(struct MessageMenu *m);
void MessageMenu_InitializeTransition(struct MessageMenu *m);
int MessageMenu_Update(struct MessageMenu *m);
int MessageMenu_Destroy(struct MessageMenu *m);
int MessageMenu_Initialize(struct MessageMenu *m);

#endif
