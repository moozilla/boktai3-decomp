#include "global.h"
#include "player.h"

#define ControlPanel_ResetPlayerForInteractionPrompt sub_081C1820

void ControlPanel_ClearPlayerInputHistory(void);

void sub_081C1820(struct Player *p)
{
    gUnk_02000580[0]->kind = 1;
    gUnk_02000580[0]->tetherMode = 0x25;
    gUnk_02000580[0]->tetherPhase = 0;
    gUnk_02000580[0]->position.x = 0x380;
    gUnk_02000580[0]->position.y = 0x100;
    gUnk_02000580[0]->position.z = 0x580;
    gUnk_02000580[0]->facing = 7;
    gUnk_02000580[0]->heading = 1;
    gUnk_02000580[0]->side = 1;
    ControlPanel_ClearPlayerInputHistory();
    gUnk_02000580[0]->interactionPromptFlags = 1;
    switch (p->interactionPromptCategory) {
    case 0:
        gUnk_02000580[0]->interactionPromptFlags |= 0x20;
        break;
    case 1:
        gUnk_02000580[0]->interactionPromptFlags |= 0x40;
        break;
    case 2:
        gUnk_02000580[0]->interactionPromptFlags |= 0x10;
        break;
    case 3:
        gUnk_02000580[0]->interactionPromptFlags |= 0x80;
        break;
    }
}
