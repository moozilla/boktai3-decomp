#include "global.h"
void *Inventory_GetEquipmentEntry(void *);
void ActionRecord_CopyFromEntry(void *, void *);
void Inventory_GetEquipmentDescriptor(void *a, void *b)
{
    ActionRecord_CopyFromEntry(Inventory_GetEquipmentEntry(a), b);
}
