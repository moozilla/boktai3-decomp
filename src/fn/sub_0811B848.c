#include "global.h"
#include "engine.h"
struct PackedVec {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
struct E7BCObject {
    u8 pad0[0x40];
    s32 field40, active;
};
extern void *gUnk_02000204;
void *sub_0811B7E8(void);
struct E7BCObject *sub_0811B828(void *);
void sub_0811B278(struct E7BCObject *, struct PackedVec *, struct PackedVec *, struct PackedVec *,
                  s32, s32, s32, s32, s32, void (*)(void), struct E7BCObject *);
void sub_0811B2FC(struct E7BCObject *);
void sub_0811B784(void);
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
   (each x y z, missing values 0), then sets it up with sub_0811B278. */
void RegistryE7BC_CreateFromScript(void)
{
    void *manager = gUnk_02000204;
    struct E7BCObject *e;
    struct PackedVec p;
    struct PackedVec s;
    struct PackedVec o;
    if (!manager) {
        manager = sub_0811B7E8();
        if (!manager)
            return;
    }
    e = sub_0811B828(manager);
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
    sub_0811B278(e, &p, &s, &o, 12, 7, 0, 6, 12, sub_0811B784, e);
    sub_0811B2FC(e);
    e->active = 1;
}
