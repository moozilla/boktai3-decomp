#ifndef GUARD_NAME_ENTRY_H
#define GUARD_NAME_ENTRY_H

#include "text_entry_widget.h"

struct NameEntryTask {
    u8 task[0x18];
    struct TextEntryWidget widgets[6];
    s32 unk348;
    s32 selection; /* 0x34C: keyboard key; 0x55 page toggle, 0x56 End */
    s32 position;  /* 0x350: cursor position in the name */
    void *bg;
    u16 verticalHistory, horizontalHistory;
    s32 verticalRepeat, horizontalRepeat;
    s16 state, result;
    s32 timer, fadeShift;
    u32 slot;      /* 0x370 */
    u8 name[0x10]; /* 0x374 */
    u8 unknown384[0x10];
    const u8 *initialName;
    u16 *savedCursorPalette, *savedKeyPalette;
    u16 keyColors[16], cursorColors[16];
    u8 unknown3E0[4];
};

s32 NameEntry_Initialize(struct NameEntryTask *);
void TextEntry_CopyInitialName(struct NameEntryTask *, u8 *);

typedef char NameEntryTaskSizeCheck[sizeof(struct NameEntryTask) == 0x3E4 ? 1 : -1];

#endif
