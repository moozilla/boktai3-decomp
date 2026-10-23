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
    u8 unk0[0x24];
    u8 mode;
    u8 category;
    u8 entry;
    u8 unk27;
    s32 windows[9];
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
    u8 unkC6C[0xD50 - 0xC6C];
    const u8 *script;
    u8 unkD54[0xF1C - 0xD54];
    void *display;
};

#endif
