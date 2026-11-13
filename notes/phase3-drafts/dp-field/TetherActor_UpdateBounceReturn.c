void TetherActor_UpdateBounceReturn(struct TetherActor *actor)
{
    u16 *frame;
    u32 zero;
    u32 frameValue;
    if (actor->state != 8)
        TetherActor_SetState(actor, 8);
    {
        u16 *glow = &actor->glowMode;
        zero = 0;
        *glow = zero;
    }
    frame = &actor->stateFrame;
    frameValue = *frame;
    if (!frameValue) {
        u16 *height;
        actor->shakeX = zero;
        actor->shakeZ = zero;
        height = &actor->displayOffsetY;
        if (*height <= 30) {
            TetherActor_RemoveAuxiliarySprite(actor);
            *height = zero;
            *frame = 1;
        } else
            *height -= 30;
        return;
    }
    {
        u32 incremented = frameValue + 1;
        u32 current;
        *frame = incremented;
        current = (u16)incremented;
        if (current <= 7) {
            current = (u16)incremented;
            if (current <= 3)
                actor->displayOffsetY = incremented << 4;
            else {
                u32 height = (7 - incremented) << 4;
                u16 *destination;
                destination = (u16 *)&actor->displayOffsetY;
                *destination = height;
            }
        } else
            actor->displayOffsetY = zero;
    }
    {
        u16 *counter = &actor->stateFrame;
        if (*counter <= 11) {
            const u16 *table = gRandomTable;
            actor->shakeX = (ParticleRandom_ReadDirectSample(table) & 31) - 16;
            actor->shakeZ = (ParticleRandom_ReadDirectSample(table) & 31) - 16;
        }
        if (*counter == 2)
            TetherActor_EmitRingEffects(actor);
        if (*counter > 29) {
            TetherActor_SetState(actor, 0);
            actor->chargeReady = 1;
            actor->charge = 287;
            actor->pendingEffectHandle =
                ActorAttachment_UpdateFromDescriptor(actor->pendingEffectHandle, &actor->bodyConfig,
                                                     1, 0, &actor->charge, 287, 250, 290);
        }
    }
}

