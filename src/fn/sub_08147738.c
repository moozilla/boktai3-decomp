#include "global.h"
#include "engine.h"

void PlayerAction_RefreshModifiers(u8 *);

void Player_OnMotion10Exit(u8 *p)
{
    if (!(gEngineFlags & 0x1000)) {
        PlayerAction_RefreshModifiers(p);
    }
    *(u8 *)(p + 0xAC8) = 0xFF;
}
