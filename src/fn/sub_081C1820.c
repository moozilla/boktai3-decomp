#include "global.h"
#include "player.h"

void ControlPanel_ClearPlayerInputHistory(void);

void ControlPanel_ResetPlayerForInteractionPrompt(struct Player *p)
{
    gPlayers[0]->kind = 1;
    gPlayers[0]->tetherMode = 0x25;
    gPlayers[0]->tetherPhase = 0;
    gPlayers[0]->position.x = 0x380;
    gPlayers[0]->position.y = 0x100;
    gPlayers[0]->position.z = 0x580;
    gPlayers[0]->facing = 7;
    gPlayers[0]->heading = 1;
    gPlayers[0]->side = 1;
    ControlPanel_ClearPlayerInputHistory();
    gPlayers[0]->interactionPromptFlags = 1;
    switch (p->interactionPromptCategory) {
        case 0:
            gPlayers[0]->interactionPromptFlags |= 0x20;
            break;
        case 1:
            gPlayers[0]->interactionPromptFlags |= 0x40;
            break;
        case 2:
            gPlayers[0]->interactionPromptFlags |= 0x10;
            break;
        case 3:
            gPlayers[0]->interactionPromptFlags |= 0x80;
            break;
    }
}
