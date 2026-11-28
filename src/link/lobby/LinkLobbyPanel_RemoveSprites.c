#include "global.h"
#include "link/lobby/record_menu.h"

void Anim_Remove(void *);

void LinkLobbyPanel_RemoveSprites(struct RecordMenu *menu)
{
    s32 i;

    for (i = 0; i < 9; i++)
        Anim_Remove(&menu->sprites[i]);
}
