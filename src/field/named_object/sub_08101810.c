#define gNamedObjectRandomIndex gRandomIndex
#include "engine/shared/global.h"
#include "engine/shared/scene_vec.h"
#include "engine/shared/engine.h"
#include "engine/shared/ram.h"
#include "field/named_object/named_object_flags.h"
#include "field/named_object/named_object.h"
s32 Div(s32, s32);
s32 Mod(s32, s32);
static inline u16 RandomValue(void)
{
    return gRandomTable[gRandomIndex = (gRandomIndex + 1) & 0x3FF];
}
void NamedObject_ApplyVerticalImpulseAndJitter(struct NamedObject *object)
{
    if (object->verticalImpulse < 0) {
        object->sprite.position.y += object->verticalImpulse;
        object->verticalImpulse = Div(object->verticalImpulse, 2);
    }
    if (NamedObjectFlags_Has(0x80, &object->flags))
        goto clear;
    if (NamedObjectFlags_Has(0x20, &object->flags)) {
        object->sprite.position.x += Mod(RandomValue(), 12) - 6;
        {
            u32 next = (gRandomIndex + 1) & 0x3FF;
            u32 value = gRandomTable[next];
            value &= 3;
            object->sprite.position.y += value;
            gRandomIndex = (next + 1) & 0x3FF;
            object->sprite.position.z += Mod(gRandomTable[gRandomIndex], 12) - 6;
        }
    clear:
        NamedObject_DisableFlags(0x20, object);
    }
}
