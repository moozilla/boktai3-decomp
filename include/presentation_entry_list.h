#ifndef PRESENTATION_ENTRY_LIST_H
#define PRESENTATION_ENTRY_LIST_H
/* Tentative list-menu view; offsets established by 08174B8C and 08174E38. */
struct PresentationEntrySource {
    u8 unknown0[0x418];
    u8 mode;
};
struct PresentationEntryList {
    u8 unknown0[0xA40];
    struct PresentationEntrySource *source;
    u8 unknownA44[0xBDC];
    u16 cursorX, cursorY;
    u8 unknown1624[0x3284];
    u16 entryCount;
    u8 rowIndex;
    u8 unknown48AB[0xB];
    u16 pendingValues[3];
};
#endif
