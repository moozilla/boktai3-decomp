/* Monster book (bestiary) screen layout, inferred from MonsterBook_Create's
 * teardown order and the list/detail redraw helpers. Field roles are tentative. */
#ifndef GUARD_MONSTER_BOOK_H
#define GUARD_MONSTER_BOOK_H

struct BookSprite {
    u8 unk0[8];
    u32 flags;
    u8 unkC[0x14];
    s16 x;
    s16 y;
    u8 unk24[0x3C];
};

struct MonsterBook {
    u8 unk0[0x20];
    u32 frame;
    u8 mode;
    u8 category;
    u8 entry;
    u8 unk27;
    s32 windows[8];
    u8 selectedRow;
    u8 unknown49[3];
    u8 animData[0x20];
    struct BookSprite s6C;
    struct BookSprite sCC;
    struct BookSprite s12C;
    struct BookSprite s18C;
    struct BookSprite s1EC;
    struct BookSprite s24C;
    struct BookSprite s2AC[7];
    struct BookSprite s54C[7][2];
    struct BookSprite sA8C[2];
    struct BookSprite sB4C;
    struct BookSprite sBAC[2];
    u8 unkC6C[0xD4E - 0xC6C];
    u16 transitionFlags;
    const u8 *script;
    u8 unkD54[0xDA7 - 0xD54];
    u8 pendingExit;
    u8 unkDA8[0xF18 - 0xDA8];
    void *closeScript;
    void *display;
};

#endif
