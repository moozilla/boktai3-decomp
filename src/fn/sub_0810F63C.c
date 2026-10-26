#include "global.h"
#include "ram.h"
void LinkLobbyPanel_RemoveSprites(void *);
void LinkLobbyPanel_ClearTextWindows(void *);
void CharacterModel_RemoveAnimations(void *);
s32 LinkLobbyPanel_OnTaskDestroy(u8 *s)
{
    s32 i;
    u8 *p;
    u32 z;
    LinkLobbyPanel_RemoveSprites(s);
    LinkLobbyPanel_ClearTextWindows(s);
    p = s + 0xa30;
    i = 1;
    do {
        CharacterModel_RemoveAnimations(p);
        p += 0x850;
    } while (--i >= 0);
    z = 0;
    gActiveRecordMenu = z;
    return 1;
}
