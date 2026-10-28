#ifndef GUARD_AREA_EFFECT_LINK_H
#define GUARD_AREA_EFFECT_LINK_H
#include "area_effect.h"
/* Observed link-session cache, identity of byte values remains provisional. */
/* Resource selector 0811C67C reads two 0x40-byte views starting at 0x120.
 * These overlap the session data and later exchange fields. */
struct AreaEffectLinkResourceSlot {
    u8 unknown0[0x22];
    u16 element;
    u8 unknown24[0x1C];
};
struct AreaEffectLinkState {
    u8 unknown0[0x18];
    s16 localPlayer;
    u16 participantCount; /* 0x1A */
    u8 unknown1C;
    u8 exchangeBusy; /* 0x1D */
    u8 unknown1E[3];
    u8 readyPhase; /* 0x21 */
    u8 unknown22;
    u8 menuEvent;  /* 0x23; session menu callbacks */
    s8 menuChoice; /* 0x24 */
    u8 unknown25[11];
    u32 menuElapsed;   /* 0x30 */
    u16 menuSelection; /* 0x34 */
    u8 unknown36[8];
    u16 requestedMode; /* 0x3E */
    u16 *selection;    /* 0x40; packed mode in bits 10-13 */
    u8 unknown44[8];
    u8 selectionPage; /* 0x4C */
    u8 previousSelectionPage;
    u8 sendBit;     /* 0x4E */
    u8 receiveBit;  /* 0x4F */
    u8 receiveBits; /* 0x50 */
    u8 unknown51[2];
    u8 unavailable;      /* 0x53 */
    u8 selectionBits[8]; /* 0x54: two pages of four participants */
    u8 playerValues[4];
    u16 playerValueSum; /* +0x60: sum of selection bytes multiplied by four, 081381CC. */
    u16 value;
    u8 unknown64[0x18];
    u16 receivedByteCounts[4]; /* +0x7C: per-participant assembly cursors, 08138300. */
    u8 unknown84[8];
    s16 menuResults[2]; /* 0x8C: selected local result in 081100A0 */
    u8 unknown90[0x24];
    void (*menuCallback)(void); /* 0xB4 */
    s32 menuTextWindow;         /* 0xB8 */
    u8 unknownBC[0xC];
    u32 readyScript; /* 0xC8 */
    u8 unknownCC[4];
    u32 confirmScript; /* 0xD0 */
    u8 unknownD4[0xC];
    u8 sessionData[0x84]; /* 0xE0 */
    s8 playerKinds[5];    /* 0x164 */
    u8 unknown169[3];
    u16 counters[4];          /* 0x16C */
    u16 alternateCounters[1]; /* 0x174; indexed extent unknown */
    u8 unknown176[0x1A0 - 0x176];
    u8 outgoingPayload[0x20]; /* 0x1A0 */
    u32 outgoingChecksum;     /* 0x1C0 */
    u8 incomingPayload[0x48]; /* 0x1C4; observed stride 0x24 */
    u16 exchangeValue;        /* 0x20C */
    u16 exchangeText;         /* 0x20E */
    u8 outgoingChoices[4];    /* 0x210 */
    u8 incomingChoices[4];    /* 0x214 */
    u8 unknown218[0x23C - 0x218];
    u32 emptySelectionScript; /* 0x23C */
    u32 selectionScript;      /* 0x240 */
    u32 menuEventScript;      /* 0x244 */
    u8 unknown248[0xC];
    const u8 *choiceMessageScript; /* 0x254 */
    u8 unknown258[4];
    const u8 *menuMessageScript; /* 0x25C */
    u8 unknown260[0x80];
    u32 positionScript; /* 0x2E0 */
};
extern struct AreaEffectLinkState *gLinkSession;
extern struct AreaEffectPlayer *gPlayers[4];
#endif
