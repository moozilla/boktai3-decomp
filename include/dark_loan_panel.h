#ifndef FOUNDRY_DARK_LOAN_PANEL_H
#define FOUNDRY_DARK_LOAN_PANEL_H
struct DarkLoanPanel {
    u8 unknown0[0x18];
    u8 spriteData[0x20];
    u8 secondarySpriteData[0x20];
    u8 animation[8];
    u32 animationFlags;
    u8 unknown64[0x54];
    u8 secondaryAnimation[8];
    u32 secondaryAnimationFlags;
    u8 unknownc4[0x5c];
    u32 spriteFlags120;
    u8 unknown124[0x5c];
    u32 spriteFlags180;
    u8 unknown184[0x5c];
    u32 spriteFlags1e0;
    u8 unknown1e4[0x6fc];
    void *map;
    u8 unknown8e4[0xa];
    u16 selection;
    u8 unknown8f0[4];
    u32 animateOpening;
    u8 unknown8f8[0xa];
    u16 secondaryVisible;
};
#endif
