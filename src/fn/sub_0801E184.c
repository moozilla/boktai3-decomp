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
    slot->kind = kind;
    slot->variant = variant;
    slot->dir = orientation;
    slot->b3a = 0;
    slot->type = type;
    slot->maxLift = maxLift;
    slot->hasArgs = hasArgs;
    slot->b41 = b41;
    slot->value = value;
    slot->lift = 0;
    slot->region = Region_FindContaining(position);
    slot->b44 = b44;
    slot->block = Heap_Alloc(16);
    if (!slot->block)
        return -1;
    Memory_Clear(slot->block, 16);
    switch (slot->type) {
        case 1:
            sub_0801F058(p, slot);
            break;
        case 2:
            slot->b45 = b45;
            SceneObject_InitSlotPlacement(p, slot);
            break;
        case 8:
            slot->b45 = 1;
            SceneObject_InitSlotPlacement(p, slot);
            break;
        case 9:
            slot->b45 = 2;
            SceneObject_InitSlotPlacement(p, slot);
            break;
        case 10:
            slot->b45 = 3;
            SceneObject_InitSlotPlacement(p, slot);
            break;
        case 11:
            slot->b45 = 4;
            SceneObject_InitSlotPlacement(p, slot);
            break;
        case 12:
            slot->b45 = 5;
            SceneObject_InitSlotPlacement(p, slot);
            break;
        case 13:
            slot->b45 = 6;
            SceneObject_InitSlotPlacement(p, slot);
            break;
        case 14:
            slot->b45 = 7;
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
    if (slot->hasArgs) {
        for (i = 0; i < 4; i++)
            slot->args.words[i] = args[i];
    } else
        slot->args.words[0] = 0;
    ++*(u16 *)(p + 0x1a);
}
