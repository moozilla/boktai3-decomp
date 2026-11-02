#include "engine.h"
#include "frost_emitter.h"

#define FrostEmitter_Initialize sub_081D3094
s32 FrostEmitter_Initialize(struct FrostEmitter *p)
{
    void *graphics = p->graphics;
    void *spriteGraphics;
    struct FrostParticle *slot;
    u16 *lifetime;
    struct FrostEmitterStatus *state;
    u32 *enabled, *age;
    u32 zero;
    u32 one;
    s32 i;
    AnimData_Load(graphics, 0x4384);
    AnimData_SelectPaletteBank(graphics, 0xB6);
    p->resource = ScriptProc_Start(0x922E, 0xF62B);
    if (!p->resource)
        return -1;
    i = 0;
    spriteGraphics = graphics;
    state = &p->runtime;
    enabled = &state->enabled;
    age = &state->age;
    one = 1;
    lifetime = &p->particles[0].lifetime;
    slot = p->particles;
    do {
        u32 flags;
        Sprite_Init(slot, spriteGraphics, 0);
        flags = slot->sprite.flags | one;
        slot->sprite.flags = flags;
        if (i & one)
            flags |= 0x200;
        else
            flags |= 0x400;
        slot->sprite.flags = flags;
        zero = 0;
        *lifetime = zero;
        slot->sprite.priority = one;
        lifetime = (u16 *)((u32)lifetime + sizeof(*slot));
        slot++;
        i++;
    } while (i <= 11);
    *enabled = zero;
    *age = zero;
    return zero;
}
