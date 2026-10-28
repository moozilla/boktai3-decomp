#include "global.h"
#include "engine.h"
#include "ram.h"
struct PackedVec {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
struct E7BCObject {
    u8 pad0[0x40];
    s32 field40, active;
};
void *RegistryE7BC_CreateManager(void);
struct E7BCObject *RegistryE7BC_FindFreeObject(void *);
void SpatialIndex_InitEntry(struct E7BCObject *, struct PackedVec *, struct PackedVec *,
                            struct PackedVec *, s32, s32, s32, s32, s32, void (*)(void),
                            struct E7BCObject *);
void SpatialIndex_InsertEntry(struct E7BCObject *);
void RegistryE7BC_OnObjectComplete(void);
static inline s32 optionalValue(void)
{
    s32 value;
    if (Script_GetPc())
        value = Script_GetValue();
    else
        value = 0;
    return value;
}
/* Registry routine E7BC: takes an object from the manager at 0x02000204
   (created on demand), -P (default 0), and three required points -p, -s, -o
   (each x y z, missing values 0), then sets it up with SpatialIndex_InitEntry. */
void RegistryE7BC_CreateFromScript(void)
{
    void *manager = gRegistryE7BCManager;
    struct E7BCObject *e;
    struct PackedVec p;
    struct PackedVec s;
    struct PackedVec o;
    if (!manager) {
        manager = RegistryE7BC_CreateManager();
        if (!manager)
            return;
    }
    e = RegistryE7BC_FindFreeObject(manager);
    if (!e)
        return;
    e->field40 = Script_GetKeywordValue('P', 0);
    if (!Script_SeekToKeyword('p'))
        return;
    p.x = optionalValue();
    p.y = optionalValue();
    p.z = optionalValue();
    if (!Script_SeekToKeyword('s'))
        return;
    s.x = optionalValue();
    s.y = optionalValue();
    s.z = optionalValue();
    if (!Script_SeekToKeyword('o'))
        return;
    o.x = optionalValue();
    o.y = optionalValue();
    o.z = optionalValue();
    SpatialIndex_InitEntry(e, &p, &s, &o, 12, 7, 0, 6, 12, RegistryE7BC_OnObjectComplete, e);
    SpatialIndex_InsertEntry(e);
    e->active = 1;
}
