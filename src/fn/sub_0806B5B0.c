#include "sprite_effect_controller.h"
/* The resource loader consumes r1 as the graphics ID; the prior one-argument
 * declaration happened to leave that argument register untouched. */
void *SpriteEffectController_LoadSpriteResources(struct SpriteEffectController *slot,
                                                 s32 graphicsId, u32 context)
{
    AnimData_Load((u8 *)slot->drawable + 0x2c, graphicsId);
    return ScriptProc_Start(0x922e, context);
}
