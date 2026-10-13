#ifndef GUARD_PLAYER_H
#define GUARD_PLAYER_H

#include "global.h"
#include "scene_vec.h"

/*
 * The player actor. gUnk_02000580 holds four pointers (one per player slot; slot 0 is the local
 * player, the other slots are used by the multiplayer code, guess). Built from the offsets seen
 * across src/ (partial views in src/fn/*.c); unknown ranges are u8 unkXXX[N]. Names are
 * provisional.
 */
struct Player {
    u8 unk0[0x20];
    u32 statusFlags; /* 0x186c: busy bits, 0x100000: cannot interact (guess) */
    u8 unk24[0xC];
    struct SceneVec position;
    u8 unk38[0x36C];
    u8 facing; /* (guess) direction index */
    u8 unk3A5[0x73];
    u8 actionMode; /* 6: alternate effect mode / locked containers refuse to open (guess) */
    u8 unk419[0x2F];
    u32 actionFlags; /* bit 4: items always visible (guess) */
    u8 unk44C[0xE8];
    u16 sceneMode; /* (guess) */
    u16 unk536;
    u16 active; /* (guess) */
};

extern struct Player *gUnk_02000580[4];

#endif
