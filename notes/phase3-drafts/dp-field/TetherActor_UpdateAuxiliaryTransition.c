#define TETHER_CAPTURE_DISPLAY(out, actor) do { (out) = &(actor)->display.animation; } while (0)

static inline u32 TetherActor_ReadAuxiliaryFlags(struct TetherActor *actor)
{
    return actor->auxiliarySprite.flags;
}

void TetherActor_UpdateAuxiliaryTransition(struct TetherActor *actor)
{
    u32 visible;
    u32 emit;
    struct DisplayAnimation *display;
    struct DisplayAnimation *activeDisplay;
    u16 *frame;
    u32 rawFrame;
    u32 highFrame;
    u32 current;
    if (actor->state != 5)
        TetherActor_SetState(actor, 5);
    if (gEngineFlags & 512)
        return;
    actor->glowMode = 2;
    actor->heading = 0;
    actor->chargeCollider.flags |= 4;
    visible = 0;
    emit = visible;
    frame = &actor->stateFrame;
    rawFrame = *frame;
    highFrame = rawFrame << 16;
    current = highFrame >> 16;
    if (current <= 63) {
        actor->position.y = (actor->position.y * 7 + actor->auxiliaryOrigin.y) >> 3;
        goto show;
    }
    if (current <= 127) {
        s32 shake;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 bit = ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
            bit &= 1;
            shake = (bit << 4) + bit - 8;
        }
        actor->shakeX = shake;
        actor->shakeZ = -shake;
        visible = 1;
        if ((*frame & 15) == 0)
            emit = visible;
    } else if (current <= 191) {
        if ((highFrame >> 17) & 3)
            visible = 1;
        {
            u32 mask = 15;
            mask &= rawFrame;
            if (!mask)
                emit = 1;
        }
    } else {
        if ((highFrame >> 17) & 1)
            visible = 1;
        {
            u32 mask = 15;
            mask &= rawFrame;
            if (!mask)
                emit = 1;
        }
    }
    if (visible) {
    show:
        TETHER_CAPTURE_DISPLAY(activeDisplay, actor);
        DisplayAnimation_ClearFlags(activeDisplay, 1);
        actor->auxiliarySprite.flags &= ~1;
        {
            struct ContactPosition *position = &actor->auxiliarySprite.position;
            s32 x, z;
            u8 *properties;
            u32 surface;
            s32 height;
            x = position->x >> 8;
            z = position->z >> 8;
            display = activeDisplay;
            properties = propertiesAt_0804C748(x, z);
            surface = *properties >> 4;
            height = (*properties & 15) << 8;
            switch (surface) {
                case 1:
                    height -= (u8)position->z;
                    break;
                case 2:
                    height -= (u8)position->x;
                    break;
            }
            actor->auxiliarySprite.position.y = height;
        }
    } else {
        TETHER_CAPTURE_DISPLAY(activeDisplay, actor);
        DisplayAnimation_SetFlags(activeDisplay, 1);
        {
            u32 *flags = &actor->auxiliarySprite.flags;
            u32 value = TetherActor_ReadAuxiliaryFlags(actor);
            value |= 1;
            *flags = value;
        }
        display = activeDisplay;
    }
    if (emit)
        TetherActor_EmitPointEffect((const struct ContactPackedVector *)&actor->position,
                                    &actor->stateOption);
    TetherActor_SetEntry(actor, 0);
    if (++actor->stateFrame > 255) {
        TetherActor_RemoveAuxiliarySprite(actor);
        TetherActor_SetState(gTetherActor, 0);
        if (actor->interactionScriptId) {
            u32 script = actor->interactionScriptId;
            actor->interactionScriptId = 0;
            Script_Call(script, 0);
            actor->destroyRequested = 1;
            DisplayAnimation_SetFlags(display, 1);
        } else {
            DisplayAnimation_SetFlags(display, 1);
            actor->destroyRequested = 1;
        }
    }
}

