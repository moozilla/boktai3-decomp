#ifndef GUARD_GAME_STATE_H
#define GUARD_GAME_STATE_H

#include "global.h"

/*
 * The main game-state block behind gUnk_02000710 (guess: save data plus world state; clearing
 * 0x30..0x77a resets a game). Built from the offsets seen across src/ (partial views in
 * src/fn/*.c). Only fields with a known access are named; unknown ranges are u8 unkXXX[N].
 * Names are provisional, "(guess)" marks unconfirmed interpretations. Array lengths are
 * bounded by the next known field, not proven.
 */
struct GameState {
    u8 unk0[0x8];
    u16 values08[5]; /* (guess) */
    u16 unk12;
    s16 unk14;
    u8 unk16[0x12];
    s16 life;    /* (guess) current; (lifeMax - life) / lifeMax is a ratio in sub_0813E208 */
    s16 lifeMax; /* (guess) */
    u16 unk2c;
    u8 unk2E[0x2];
    s16 positionX, positionY,
        positionZ; /* (guess) saved player position; Memory_Clear(+0x30, 0x74a) resets the save-dependent part */
    s16 unk36;
    u8 unk38[0x8];
    s16 level; /* (guess) compared against actor levels */
    u8 unk42[0xA];
    u32 unlockMask; /* (guess) bit set per index */
    u16 unk50;      /* set to 8 on reset */
    s16 selected;   /* (guess) index into the slot tables */
    u8 unk54[0x4];
    s16 party[4]; /* (guess) ids, searched by sub_0822E254 */
    u8 unk60[0x8];
    s16 inventory[16]; /* (guess) -1 is empty */
    s16 slots88[12];   /* (guess) indexed by `selected` */
    s16 table0A0[48];  /* (guess) */
    s16 table100[48];  /* (guess) bit 15 is a flag */
    s16 table160[432]; /* (guess) -1 is empty */
    s16 table4C0[16];  /* (guess) indexes gUnk_08E60F48 */
    u32 unk4E0;
    s32 unk4E4;
    s32 unk4E8;
    u32 unk4EC;
    u32 date; /* (guess) day number from the RTC (sub_08228D7C) */
    s32 hour; /* (guess) RTC time at the last check */
    s32 minute;
    s32 second;
    u8 unk500[0x1C];
    s32 unk51C, unk520; /* encoded record fixed-point fields; meanings tentative */
    u8 unk524[0x12];
    s16 unk536;
    u16 counter538; /* (guess) capped at 9999 */
    u8 unk53A[0x28];
    u16 counter562; /* (guess) capped at 9999 */
    u8 unk564[0x2];
    u16 counter566; /* (guess) capped at 9999 */
    u8 unk568[0x2];
    u16 counter56A; /* (guess) capped at 9999 */
    u16 counter56C; /* (guess) capped at 9999 */
    u8 unk56E[0x6];
    u16 counter574; /* (guess) capped at 9999 */
    u16 counter576; /* (guess) capped at 9999 */
    u8 unk578[0x4];
    u16 counter57C; /* (guess) capped at 9999 */
    u8 unk57E[0x18];
    s16 flags596; /* (guess) bit mask, bits 0-9 */
    u8 unk598[0x8];
    s32 region; /* current region/scene id (guess); compared by interactable controllers */
    u32 unk5A4;
    s16 form; /* (guess) selects behavior in sub_08032300 */
    s16 unk5AA;
    u16 savedX; /* (guess) position copied by sub_0818D754 */
    u16 savedY;
    u16 savedZ;
    u16 unk5B2; /* set from a script value */
    u8 unk5B4[0x4];
    u32 unk5B8;
    u32 unk5BC;
    u8 unk5C0[0x40];
    u32 unk600;
    u8 unk604[0xC];
    u16 unk610;
    s16 unk612;
    s32 unk614;
    s32 unk618;
    s32 unk61C;
    u8 unk620[0x8];
    s32 counters628[4]; /* (guess) indexed by idx */
    u8 unk638[0x28];
    s16 unk660, unk662, unk664; /* encoded record fields; meanings tentative */
    u8 unk666[0xBA];
    u32 flags720; /* (guess) event flag words: bits 0-31 */
    u32 flags724; /* (guess) bits 32-63 */
    u32 flags728;
    u32 flags72C;
    u32 flags730;
    u8 unk734[0x10];
    u16 unk744;
    u8 unk746[0x12];
    s16 unk758;
    s16 unk75A;
    u8 unk75C[2];
    u8 unk75E, unk75F;
    u8 unk760[0x1C];
    s32 unk77C;
    u8 playerName[32]; /* (guess) */
    u16 unk7A0[4];
    u8 modifierIdsBytes[8]; /* tentative: four packed s16 source IDs; 0803F95C/0803FDBC */
    s16 unk7B0;
    u8 unk7B2[2];
    s16 unk7B4;
    u8 unk7B6[2];
    u32 unk7B8[4];
    s32 unk7C8[12];
    s16 unk7F8;
    u8 unk7FA[0x6];
    s16 unk800[13];
    s16 unk81A;
    u32 unk81C;
    u8 unk820[0x8];
    s16 unk828[8];
    u16 unk838[16];
    u8 unk858[0x6];
    s16 unk85E;
    u8 unk860[0x8];
    u32 flags868; /* (guess) bit 1: feature enabled */
    u16 unk86C;   /* also read as a u32 by sub_0811A604 */
    u16 flag86E;
    u8 unk870[0x4];
    s16 unk874;
    s16 unk876;
    u8 unk878[0x2];
    s16 unk87A;
    u16 counter87C; /* (guess) capped at 9999 */
    u8 unk87E[0xE];
    s32 unk88C;
    s16 unk890;
    s16 unk892;
    s16 unk894;
};

extern struct GameState *gUnk_02000710;

#endif
