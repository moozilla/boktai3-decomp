#include "engine/video/display_sequence_sprite.h"
#include "monsters/sword/sword_task.h"

void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetBodyProperties(void *, s32, s32, s32);
void Collider_SetCallback(void *, void *, void *);
s32 Collider_Refresh(void *);
static inline s32 ReadArgument(void)
{
    return Script_GetPc() ? Script_GetValue() : 0;
}
void Ivy_OnChildDestroy(void);
void Ivy_OnChildContact(void);
static inline void Sword_ClearByte(u8 *field)
{
    *field = 0;
}

void Sword_InitAttackComponents(struct SwordTask *p)
{
    struct ContactPackedVector size;
    struct ContactPackedVector offset;
    struct DisplaySequenceState sequence;
    s32 i;
    size.x = 100;
    size.y = 100;
    size.z = 100;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    if (Script_SeekToKeyword('c')) {
        p->fieldD14 = ReadArgument();
        p->bodyD16 = ReadArgument();
        p->fieldD18 = ReadArgument();
        p->fieldD1A = ReadArgument();
        p->fieldD7E = ReadArgument();
        p->bodyD1C = ReadArgument();
        p->bodyD1E = ReadArgument();
    }
    if (Script_SeekToKeyword('C')) {
        p->attackD20 = ReadArgument();
        p->attackD22 = ReadArgument();
        p->attackD24 = ReadArgument();
        p->attackD26 = ReadArgument();
    }
    if (Script_SeekToKeyword('n'))
        p->count = Script_GetValue();
    else
        p->count = 4;
    Anim_SetSequence(&sequence, (void *)ScriptProc_Start(0x922E, 0x3F72), 2,
                     (u8)((p->variant) << 2), 0);
    /* Advance the same sequence once for each component. */
    for (i = 0; i < 4; i++) {
        struct SwordComponent *child = &p->components[i];
        struct DisplaySequenceSprite *sprite = &child->sprite;
        void *graphics = &child->graphics;
        void *collider;
        child->fieldF0 = 0;
        child->fieldF2 = 0;
        child->fieldF4 = 0;
        child->fieldF8 = 0;
        child->fieldFA = 1;
        child->fieldFC = 0;
        child->entryFlags = 0;
        child->field10A = 0;
        child->angle = 0;
        Sword_ClearByte(&child->field12A);
        Sword_ClearByte(&child->field12B);
        child->field126 = 0;
        child->field128 = 0;
        child->field168 = 0;
        AnimData_Load(graphics, 0x5291);
        Sprite_Init(child, graphics, 0);
        DisplaySequence_StepSprite(sprite, &sequence);
        sprite->flags = 3;
        AnimData_SelectPaletteBank((void *)sprite->resource, p->paletteBank);
        child->index = i;
        child->active = 0;
        child->phase = 0;
        child->entryFlags |= 1;
        child->parent = p;
        child->field180 = 20;
        child->field184 = 0;
        collider = &child->attack;
        Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
        Collider_SetCallback(collider, Ivy_OnChildDestroy, child);
        Collider_Refresh(collider);
        Collider_SetAttackProperties(collider, p->attackD20, p->attackD22, 0x2000, p->attackD24,
                                     p->attackD26);
        child->attack.flags |= 4;
        collider = &child->body;
        Collider_Init(collider, 0, 0x5009, 0, 16, &size, &offset);
        Collider_SetBodyProperties(collider, p->bodyD16, p->bodyD1C, p->bodyD1E);
        Collider_SetCallback(collider, Ivy_OnChildContact, child);
        Collider_Refresh(collider);
        child->body.flags |= 4;
    }
}
