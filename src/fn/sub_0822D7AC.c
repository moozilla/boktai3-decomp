#include "global.h"
void *Inventory_GetEquipmentEntry(void *);
void sub_0822D764(void *, void *);
void Inventory_GetEquipmentDescriptor(void *a, void *b)
{
    sub_0822D764(Inventory_GetEquipmentEntry(a), b);
}
