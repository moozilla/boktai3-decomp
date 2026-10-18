#include "global.h"
#include "area_effect_link.h"
static inline u8 IsByte(u16 word)
{
    if ((word & 0x3C00) == 0x800)
        return 1;
    return 0;
}
static inline u16 GetWord(struct AreaEffectLinkState *session, s32 player)
{
    if (player < 0)
        return 0xFFFF;
    return session->selection[player];
}
s32 LinkSession_ReceiveSelectionByte(struct AreaEffectLinkState *session, u8 *destination, s32 player, s32 length)
{
    u32 word;
    u16 *cursor;
    u8 *out;
    if (session->receivedByteCounts[player] >= length)
        return 1;
    out = destination + session->receivedByteCounts[player];
    word = GetWord(session, player);
    if (IsByte(word)) {
        u32 sequence = 0x300 & word;
        u32 count;
        {
            u32 index = player * 2;
            u32 offset = 0x7C;
            u8 *base = (u8 *)session + offset;
            cursor = (void *)(base + index);
        }
        count = *cursor;
        {
            u32 mask = 3;
            sequence >>= 8;
            if (sequence == (mask & count)) {
                *out = word;
                *cursor = count + 1;
                if (*cursor >= length)
                    return 1;
            }
        }
    }
    return 0;
}
