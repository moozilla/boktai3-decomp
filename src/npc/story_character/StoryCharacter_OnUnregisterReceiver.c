#include "global.h"

u32 SceneMessages_UnregisterReceiver(u8 *);

u32 StoryCharacter_OnUnregisterReceiver(u32 a, u8 *b)
{
    return SceneMessages_UnregisterReceiver(b + 0x168);
}
