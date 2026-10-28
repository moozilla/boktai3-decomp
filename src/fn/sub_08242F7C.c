#include "global.h"
#include "player.h"

struct PlayerSelection {
    u8 unknown0[0xA];
    u16 playerMask;
};

struct Player *Player_FindMaskedSlot(const struct PlayerSelection *selection)
{
    s32 i = 0;
    struct Player **base = gPlayers;
    s32 mask = selection->playerMask;
    s32 one = 1;
    struct Player **slot = base;
    struct Player *result;
    do {
        if ((mask >> i) & one) {
            result = *slot;
            goto done;
        }
        slot++;
        i++;
    } while (i <= 3);
    result = *base;
done:
    return result;
}
