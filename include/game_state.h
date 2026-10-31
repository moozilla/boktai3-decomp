#ifndef GUARD_GAME_STATE_H
#define GUARD_GAME_STATE_H

#include "global.h"

/*
 * The main game-state block behind gGameState (guess: save data plus world state; clearing
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
    u8 unk16[2];
    u16 baseStats[3]; /* 0x18: combined with stat bonuses by 0813B4C8 */
    u8 unk1E[2];
    u16 statBonuses[3]; /* 0x20: added to normal and alternate-mode stats */
    u8 unk26[2];
    s16 life; /* (guess) current; (lifeMax - life) / lifeMax is a ratio in Player_ModifyValueByRemainingLife */
    s16 lifeMax; /* (guess) */
    u16 unk2c;
    u8 unk2E[0x2];
    s16 positionX, positionY,
        positionZ; /* (guess) saved player position; Memory_Clear(+0x30, 0x74a) resets the save-dependent part */
    s16 unk36;
    u16 alternateStatA, alternateStatB; /* 0x38: saved alternate-mode current stats */
    u8 unk3C[4];
    s16 level; /* (guess) compared against actor levels */
    u8 unk42[6];
    s32 rewardTotal;         /* 0x48: capped at 999999; tentative reward accumulator */
    u32 unlockMask;          /* (guess) bit set per index */
    u16 unk50;               /* set to 8 on reset */
    s16 selected;            /* (guess) index into the slot tables */
    s16 actionAuxiliarySlot; /* 0x54: selects the auxiliary action descriptor (tentative) */
    u8 unk56[2];
    s16 party[4];       /* (guess) ids, searched by GameState_HasEquippedConsumable */
    s16 abilityVariant; /* 0x60: selects alternate-mode ability four or five (tentative) */
    u8 unk62[6];
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
    u32 date; /* (guess) day number from the RTC (SunClock_GetPackedDate) */
    s32 hour; /* (guess) RTC time at the last check */
    s32 minute;
    s32 second;
    u8 unk500[2];
    s16 playerParticleRate; /* 0x502: nonzero enables player particles; above 2499 doubles cadence */
    u8 unk504[8];
    s32 menuRewardBalance; /* 0x50C: confirmation adds 50 per count, capped at 9999 (tentative). */
    u8 unk510[0xC];
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
    u8 unk57E[0xC];
    s16 targetFlagBonuses[5]; /* 0x58A: flag 7..11 modifier bonuses, units of 1/64 (tentative) */
    u8 unk594[2];
    s16 flags596; /* (guess) bit mask, bits 0-9 */
    u8 unk598[0x8];
    s32 region; /* current region/scene id (guess); compared by interactable controllers */
    u32 unk5A4;
    s16 form; /* (guess) selects behavior in TextState_GetPairSpeakerLabel */
    s16 unk5AA;
    u16 savedX; /* (guess) position copied by SpinningApproach_InitMotion */
    u16 savedY;
    u16 savedZ;
    u16 unk5B2; /* set from a script value */
    u8 unk5B4[0x4];
    u32 unk5B8;
    u32 unk5BC;
    u8 unk5C0[0x40]; /* Map-marker flag words, indexed by signed savedX (tentative). */
    u32 unk600;
    s32 contactThreshold; /* 0x604: randomized 200..299 after player threshold event (tentative) */
    u32 statHundredths;   /* 0x608: restored by player initialization */
    u8 unk60C[4];
    u16 unk610;
    s16 unk612;
    s32 unk614;
    s32 unk618;
    s32 unk61C;
    s32 counters620[2]; /* tentative: elapsed ticks for a player timed script event */
    s32 counters628[4]; /* (guess) indexed by idx */
    s32 modeTwoInputTicks, modeOneInputTicks; /* 0x638: input-held ticks, tentative */
    s32 counters640[5]; /* 08228090: compared after division by 54000; meaning tentative. */
    u8 unk654[0xC];
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
    s16 clockDisplayValue; /* 0x7B2: printed below base hour/minute, tentative */
    s16 unk7B4;
    u8 unk7B6[2];
    u32 unk7B8[4];
    s32 unk7C8[12];
    s16 unk7F8;
    u8 unk7FA[0x4];
    u16 styleParameterE; /* 0x7FE: copied by SceneActorStyle initializer. */
    s16 unk800[13];
    s16 unk81A;
    u32 unk81C;
    u8 unk820[0x8];
    s16 unk828[8];
    u16 unk838[16];
    u8 unk858[4];
    u16 savedTetherMode; /* 0x85C: copied by player-state snapshot (tentative) */
    s16 unk85E;
    u8 unk860[0x8];
    u32 flags868; /* (guess) bit 1: feature enabled */
    u16 unk86C;   /* also read as a u32 by Glove_UpdateTask */
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

/* 0x02000710 */
extern struct GameState *gGameState;

/* Backup of gGameState, and the active/backup pair of the shared
 * script-variable region (0x400 bytes each); gScriptLocalVariables is the separate,
 * unbacked local-variable region. Selected together by GameState_SelectRamBase. */

#endif
