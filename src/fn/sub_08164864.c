#include "global.h"
#include "ram.h"
#include "render.h"
#include "engine_globals.h"

struct HudSlotSource {
    u8 unknown0[0x18];
    s16 index;
    u8 unknown1A[0x5C - 0x1A];
    u8 counts[4];
};
extern struct HudSlotSource *gLinkSession;
extern u8 *gGameState;
u16 *Hud_GetTilemapCell(s32, u32, u32);
void Sound_PlaySong(u32);

#define OWNER   (*(u8 **)(hud + 0xA40))
#define MODE    (hud[0xA5E])
#define PULSE   (*(u16 *)(hud + 0xA64))
#define BLINK   (*(u16 *)(hud + 0xA86))
#define TIMEDUP (hud[0xAFE])

static inline s32 OwnerIndex(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->index;
}

static inline u8 SlotCount(s32 slot)
{
    if (!gLinkSession || slot < 0)
        return 0;
    return gLinkSession->counts[slot];
}

static inline u32 testFlags(const u32 *flags, u32 mask)
{
    return *flags & mask;
}

static inline u8 testFlag(u32 mask, u32 *flags)
{
    if (*flags & mask)
        return TRUE;
    return FALSE;
}

static inline u32 LinkFlags(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}

void sub_08164864(u8 *hud, s32 mode, u32 x, u32 y)
{
    s32 blink;
    u16 *tiles;
    s32 i;
    u32 pulse;
    s32 count;

    if (OWNER[0x457] == 0x23 && MODE != 8)
        goto blank;
    if (*(s16 *)(gGameState + 0x502) > 0x9C3 && MODE != 8) {
        u16 *timer = &BLINK;
        u32 bit = (*timer >> 4) & 1;
        blink = 1;
        if (bit)
            blink = 0;
        if ((*timer & 0x3F) == 2)
            Sound_PlaySong(0x132);
        (*timer)++;
    } else if (*(s16 *)(gGameState + 0x502) > 0 && MODE != 8) {
        u16 *timer = &BLINK;
        u32 bit = (*timer >> 5) & 1;
        blink = 1;
        if (bit)
            blink = 0;
        if ((*timer & 0x3F) == 2)
            Sound_PlaySong(0x131);
        (*timer)++;
    } else {
        blink = 0;
        BLINK = blink;
    }
    if (blink) {
    blank:
        tiles = Hud_GetTilemapCell(0, x, y);
        for (i = 0; i < 12; i++)
            *tiles++ = (i + 3) | 0xD000;
    } else {
        tiles = Hud_GetTilemapCell(0, x, y);
        if (mode == 1)
            pulse = 0x29F;
        else if (testFlag(0x80, (u32 *)(gGameState + 0x868)) || MODE == 8 ||
                 testFlags((u32 *)(OWNER + 0x20), 0x10)) {
            u32 frame = PULSE;
            pulse = 0x29F;
            if (frame > 0x24) {
                pulse = 0x1FB;
                if (frame <= 0x2C)
                    goto pulse_ready;
                if (frame <= 0x34)
                    pulse = 0x177;
            }
        pulse_ready:
            if (++PULSE > 0x3B)
                PULSE = 0;
        } else {
            PULSE = 0x34;
            pulse = 0x177;
        }
        if (gBgPaletteBank13Color10 != pulse)
            gBgPaletteBank13Color10 = pulse;
        if (LinkFlags(0x800))
            count = SlotCount(OwnerIndex());
        else
            count = *(s16 *)(gGameState + 0x876);
        *tiles++ = 0xD45F;
        for (i = 0; i < count; i++)
            *tiles++ = 0xD05D;
        for (; i <= 9; i++)
            *tiles++ = 0xD05E;
        *tiles = 0xD05F;
    }
    if ((u8)(MODE - 8) > 1) {
        if (*(s32 *)(gGameState + OWNER[0x472] * 4 + 0x628) > 0) {
            tiles = Hud_GetTilemapCell(0, 15, 17);
            for (i = 0; i < 5; i++)
                *tiles++ = (i + 0x3B) | 0xD000u;
            TIMEDUP = 1;
        } else if (TIMEDUP) {
            tiles = Hud_GetTilemapCell(0, 15, 17);
            for (i = 0; i < 5; i++)
                *tiles++ = 0xD000;
            TIMEDUP = 0;
        }
    }
}
