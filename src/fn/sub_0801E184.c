#include "global.h"
#include "scene_object_slot.h"
void *Heap_Alloc(int);
void Memory_Clear(void *, int);
int Region_FindContaining(void *);
void sub_0801F058(void *, void *);
void SceneObject_InitSlotPlacement(void *, void *);
void SceneObject_InitPlacement(void *, void *);
int SceneObjects_Append(u8 *p, u16 id, struct Pair *position, int value, u8 kind, u8 variant,
                        u8 orientation, int type, int maxLift, int hasArgs, int b41, s32 *args,
                        u8 b44, int b45)
{
    struct SceneObjSlot *slot;
    int i;
    if (*(u16 *)(p + 0x1a) >= *(int *)(p + 0x30))
        return -1;
    slot = *(struct SceneObjSlot **)(p + 0x42c) + *(u16 *)(p + 0x1a);
    slot->id = id;
    slot->pos.words = *position;
    slot->r0.words = *position;
    slot->activationFlags = kind;
    slot->variant = variant;
    slot->dir = orientation;
    slot->poseFrame = 0;
    slot->type = type;
    slot->maxLift = maxLift;
    slot->interactionScript = hasArgs;
    slot->autoActivate = b41;
    slot->animationIndex = value;
    slot->lift = 0;
    slot->region = Region_FindContaining(position);
    slot->hideOutsideRegion = b44;
    slot->terrain = Heap_Alloc(16);
    if (!slot->terrain)
        return -1;
    Memory_Clear(slot->terrain, 16);
    switch (slot->type) {
        case 1:
            sub_0801F058(p, slot);
            break;
        case 2:
            slot->paletteBank = b45;
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
    ++*(u16 *)(p + 0x1a);
}
