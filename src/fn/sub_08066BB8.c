#include "actor_attachment_manager.h"
#define ActorAttachment_LoadManagerResources sub_08066BB8
void ActorAttachment_LoadManagerResources(struct ActorAttachmentManagerResources *manager)
{
    AnimData_Load(manager->graphicsA, 0x363C);
    AnimData_Load(manager->graphicsB, 0x363D);
    manager->partResource = AnimData_FindEntry(0x1C1C);
    manager->sequenceA = ScriptProc_Start(0x922E, 0x6830);
    manager->sequenceB = ScriptProc_Start(0x922E, 0xD41F);
}
