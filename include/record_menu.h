#ifndef FOUNDRY_RECORD_MENU_H
#define FOUNDRY_RECORD_MENU_H
struct RecordMenuSprite {
    u8 unknown[0x20];
    u16 x, y;
    u8 pad24[0x16];
    u16 flags;
    u8 pad3c[12];
    void *tiles;
    u8 pad4c[0x14];
};
struct RecordMenu {
    u8 task[0x18];
    void *backgroundMap;
    void *otherMap;
    void *paletteResource;
    struct RecordMenuSprite sprites[9];
    u8 resource[0x20];
    u8 rowResource[0x20];
    u16 spriteIds[9];
    u16 states[9];
    void *rowProc;
    void *cursorProc;
    u8 pad3f0[4];
    u16 palette[16];
    u8 textPanel[0x61c];
    u8 panelStorage[0xaa8];
    void (*update)(struct RecordMenu *);
    u32 titleScript, choiceScript, recordScript;
    u8 pad14e8[4];
    u8 leaving, selection;
    u8 pad14ee[14];
    u8 redraw;
    u8 pad14fd;
    u16 inputDelay;
    u8 pad1500[2];
    u16 exitScript;
};
#endif
