#include "scene_object_controller.h"
int Region_FindContaining(void *);
void sub_0801F058(struct SceneObjPool *, struct SceneObjSlot *);
void SceneObject_InitSlotPlacement(struct SceneObjPool *, struct SceneObjSlot *);
void SceneObject_InitPlacement(struct SceneObjPool *, struct SceneObjSlot *);
int SceneObjects_Append(struct SceneObjPool *p, u16 id, struct Pair *position, int animationIndex,
                        u8 activationFlags, u8 variant, u8 orientation, int type, int maxLift,
                        int interactionScript, int autoActivate, s32 *args, u8 hideOutsideRegion,
                        int paletteBank)
{
    struct SceneObjSlot *slot;
    int i;
    if (p->count >= p->capacity)
        return -1;
    slot = &p->slots[p->count];
    slot->id = id;
    slot->pos.words = *position;
    slot->r0.words = *position;
    slot->activationFlags = activationFlags;
    slot->variant = variant;
    slot->dir = orientation;
    slot->poseFrame = 0;
    slot->type = type;
    slot->maxLift = maxLift;
    slot->interactionScript = interactionScript;
    slot->autoActivate = autoActivate;
    slot->animationIndex = animationIndex;
    slot->lift = 0;
    slot->region = Region_FindContaining(position);
    slot->hideOutsideRegion = hideOutsideRegion;
    slot->terrain = Heap_Alloc(sizeof(*slot->terrain));
    if (!slot->terrain)
        return -1;
    Memory_Clear(slot->terrain, sizeof(*slot->terrain));
    switch (slot->type) {
        case 1:
            sub_0801F058(p, slot);
            break;
        case 2:
            slot->paletteBank = paletteBank;
            SceneObject_InitSlotPlacement(p, slot);
            break;
        case 8:
            slot->paletteBank = 1;
            SceneObject_InitSlotPlacement(p, slot);
            break;
        case 9:
            slot->paletteBank = 2;
            SceneObject_InitSlotPlacement(p, slot);
            break;
        case 10:
            slot->paletteBank = 3;
            SceneObject_InitSlotPlacement(p, slot);
            break;
        case 11:
            slot->paletteBank = 4;
            SceneObject_InitSlotPlacement(p, slot);
            break;
        case 12:
            slot->paletteBank = 5;
            SceneObject_InitSlotPlacement(p, slot);
            break;
        case 13:
            slot->paletteBank = 6;
            SceneObject_InitSlotPlacement(p, slot);
            break;
        case 14:
            slot->paletteBank = 7;
            SceneObject_InitSlotPlacement(p, slot);
            break;
        case 0:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 15:
            SceneObject_InitPlacement(p, slot);
            break;
        case 16:
            SceneObject_InitSlotPlacement(p, slot);
            break;
    }
    if (slot->interactionScript) {
        for (i = 0; i < 4; i++)
            slot->args.words[i] = args[i];
    } else
        slot->args.words[0] = 0;
    ++p->count;
}
