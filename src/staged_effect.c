#include "staged_effect.h"
#include "global.h"
#include "game_state.h"

void StagedEffect_BlendPalette(struct StagedEffect *effect, s32 first, s32 second, s32 blend)
{
    const u16 *a = (u16 *)gPaletteBankData + (first + 0x283) * 16;
    const u16 *b = (u16 *)gPaletteBankData + (second + 0x283) * 16;
    u16 *out;
    s32 inverse;
    s32 i;
    if (blend == 0) {
        effect->activePalette = a;
    } else if (blend == 64) {
        effect->activePalette = b;
    } else {
        out = effect->blendedPalette;
        inverse = 64 - blend;
        i = 15;
        do {
            s32 redA = *a & 0x1F;
            s32 greenA = *a & 0x3E0;
            s32 blueA = *a & 0x7C00;
            s32 redB = *b & 0x1F;
            s32 greenB = *b & 0x3E0;
            s32 blueB = *b & 0x7C00;
            redA = (inverse * redA + blend * redB) >> 6;
            greenA = (inverse * greenA + greenB * blend) >> 6;
            blueA = (inverse * blueA + blueB * blend) >> 6;
            redA &= 0x1F;
            greenA &= 0x3E0;
            blueA &= 0x7C00;
            *out = redA | greenA | blueA;
            a++;
            b++;
            out++;
        } while (--i >= 0);
        effect->activePalette = effect->blendedPalette;
    }
}

void StagedEffect_MarkOuterSprites(struct StagedEffect *effect)
{
    u32 mask = 1;
    s32 remaining;
    struct DisplaySequenceSprite *sprite = &effect->sprites[1];
    for (remaining = 3; remaining >= 0; remaining--) {
        sprite->flags |= mask;
        sprite++;
    }
}

void StagedEffect_MarkOuterSpritesAlternate(struct StagedEffect *effect)
{
    u32 mask = 1;
    s32 remaining;
    struct DisplaySequenceSprite *sprite = &effect->sprites[1];
    for (remaining = 3; remaining >= 0; remaining--) {
        sprite->flags |= mask;
        sprite++;
    }
}

s32 StagedEffect_CheckScriptCompletion(struct StagedEffect *effect)
{
    struct SceneEventReceiver *receiver = &effect->receiver.messages;
    s32 index = 0;
    if (index < receiver->count[gSceneEventDispatchState->phase]) {
        do {
            struct SceneEventDispatchState *dispatch = gSceneEventDispatchState;
            struct SceneEventCommand *command = receiver->pending[dispatch->phase][index];
            SceneMessages_SetPendingTrigger(receiver, command);
            if (command->kind == 3)
                return 1;
            index++;
        } while (index < receiver->count[gSceneEventDispatchState->phase]);
    }
    return 0;
}

s32 StagedEffect_CheckScriptCompletion(struct StagedEffect *);

void StagedEffect_WaitForScript(struct StagedEffect *effect)
{
    if (StagedEffect_CheckScriptCompletion(effect)) {
        u16 *state = &effect->state;
        u32 mask = ~1;
        struct DisplaySequenceSprite *sprite = &effect->sprites[1];
        do {
            sprite->flags &= mask;
            sprite++;
        } while ((s32)sprite <= (s32)&effect->sprites[4]);
        {
            u32 zero = 0;
            *state = 1;
            effect->frame = zero;
        }
        Sound_PlaySong(0x1C6);
    }
}

void StagedEffect_ContractSpread(struct StagedEffect *effect)
{
    u16 *q = (u16 *)&effect->spread;
    u32 v = *q - 0x46;
    u32 z = 0;
    *q = v;
    if ((s32)(v << 16) <= 0) {
        u32 two;
        *q = z;
        two = 2;
        q = &effect->state;
        *q = two;
        effect->frame = z;
    } else {
        effect->frame += 1;
    }
}

void StagedEffect_ShakeAndReveal(struct StagedEffect *effect)
{
    u32 phase;
    if (effect->frame <= 3) {
        u16 *random = (u16 *)RANDOM_TABLE_ADDR;
        u32 *indexCursor;
        u32 index = (gRandomIndex + 1) & 0x3FF;
        u32 sample;
        u32 mask;
        indexCursor = &index;
        *(&sample) = random[*indexCursor];
        mask = 31;
        sample &= mask;
        sample -= 16;
        effect->shakeX = sample;
        index = (index + 1) & 0x3FF;
        gRandomIndex = index;
        sample = random[index];
        sample &= mask;
        sample -= 16;
        effect->shakeZ = sample;
    }
    phase = effect->frame & 3;
    if (phase == 0)
        StagedEffect_MarkOuterSpritesAlternate(effect);
    else if (phase == 2)
        StagedEffect_MarkOuterSprites(effect);
    if (effect->frame > 15) {
        effect->sprites[0].flags &= ~1;
        {
            u32 state = 3;
            effect->state = state;
        }
        effect->frame = 0;
    } else
        effect->frame++;
}

void AnimatedParticles_EmitRing(void *, u32, u32, u32, u32, u32, u32, u32);

void StagedEffect_ContractBurst(struct StagedEffect *effect)
{
    u32 phase;
    u16 *travel;
    u32 value;
    u32 zero;
    effect->scale = Div(effect->travel * 1008, effect->initialTravel) + 1024;
    phase = effect->frame & 3;
    if (phase == 0)
        StagedEffect_MarkOuterSpritesAlternate(effect);
    else if (phase == 2)
        StagedEffect_MarkOuterSprites(effect);
    travel = (u16 *)&effect->travel;
    value = *travel - 70;
    zero = 0;
    *travel = value;
    if ((s32)(value << 16) <= 0) {
        *travel = zero;
        effect->scale = 1024;
        {
            u32 state = 4;
            u16 *destination = &effect->state;
            *destination = state;
        }
        effect->frame = zero;
        Sound_PlaySong(0x1C4);
        StagedEffect_MarkOuterSprites(effect);
        AnimatedParticles_EmitRing(&effect->sprites[0].position, 8, 1, 1, 64, 12, 40, 250);
        if (effect->hasScript)
            Trigger_ConsumePending(&effect->receiver, 1);
    } else
        effect->frame++;
}

void Camera_RequestShake(u32);

void StagedEffect_BlendAndShake(struct StagedEffect *effect)
{
    u16 *random = (u16 *)RANDOM_TABLE_ADDR;
    u32 *indexCursor;
    u32 index = (gRandomIndex + 1) & 0x3FF;
    u32 sample = random[index];
    u32 mask = 31;
    sample &= mask;
    sample -= 16;
    effect->shakeX = sample;
    index = (index + 1) & 0x3FF;
    gRandomIndex = index;
    indexCursor = &index;
    *(&sample) = random[*indexCursor];
    sample &= mask;
    sample -= 16;
    effect->shakeZ = sample;
    {
        u32 frame = effect->frame;
        if (frame <= 3) {
            Camera_RequestShake(40);
            StagedEffect_BlendPalette(effect, 10, 8, effect->frame << 4);
        } else
            StagedEffect_BlendPalette(effect, 8, 10, (frame - 4) << 4);
    }
    if (effect->frame > 7) {
        effect->shakeX = 0;
        effect->shakeZ = 0;
        {
            u32 state = 5;
            u16 *destination = &effect->state;
            *destination = state;
        }
        effect->frame = 0;
    } else
        effect->frame++;
}

void StagedEffect_WaitBetweenShakes(struct StagedEffect *effect)
{
    if (effect->frame > 89) {
        u32 state = 6;
        effect->state = state;
        effect->frame = 0;
    } else
        effect->frame++;
}

void StagedEffect_PlayShakeSequence(struct StagedEffect *effect)
{
    u32 frame = effect->frame;
    if (frame - 10 <= 19 || frame - 60 <= 19) {
        u16 *random = (u16 *)RANDOM_TABLE_ADDR;
        u32 *indexCursor;
        u32 index = (gRandomIndex + 1) & 0x3FF;
        u32 sample;
        u32 mask;
        indexCursor = &index;
        *(&sample) = random[*indexCursor];
        mask = 31;
        sample &= mask;
        sample -= 16;
        effect->shakeX = sample;
        index = (index + 1) & 0x3FF;
        gRandomIndex = index;
        sample = random[index];
        sample &= mask;
        sample -= 16;
        effect->shakeZ = sample;
    } else {
        effect->shakeX = 0;
        effect->shakeZ = 0;
    }
    if (effect->frame == 10 || effect->frame == 20 || effect->frame == 60 || effect->frame == 70)
        Sound_PlaySong(0xD3);
    if (effect->frame > 89) {
        u32 state = 7;
        effect->state = state;
        effect->frame = 0;
    } else
        effect->frame++;
}

void StagedEffect_ReleaseScript(struct StagedEffect *effect)
{
    if (effect->frame == effect->scriptReleaseFrame) {
        if (effect->pendingScriptId) {
            Script_Call(effect->pendingScriptId, 0);
            effect->pendingScriptId = 0;
        }
    }
    effect->frame++;
}

extern void (*const gUnk_08605A58[])(struct StagedEffect *);

s32 StagedEffect_Tick(struct StagedEffect *effect)
{
    s32 i;
    gUnk_08605A58[effect->state](effect);
    if (effect->state != 0) {
        i = 0;
        do {
            struct DisplaySequenceSprite *sprite = &effect->sprites[i];
            s32 spread;
            sprite->position = *(struct ContactPosition *)&effect->position;
            spread = effect->spread;
            switch (i) {
                case 0:
                    sprite->position.y += effect->travel;
                    {
                        u32 scale = effect->scale >> 4;
                        sprite->palette = scale;
                        sprite->paletteVariant = scale;
                    }
                    break;
                case 1:
                    sprite->position.x -= spread;
                    sprite->position.z -= spread;
                    break;
                case 2:
                    sprite->position.x += spread;
                    sprite->position.z += spread;
                    break;
                case 3:
                    sprite->position.x -= spread;
                    sprite->position.z += spread;
                    break;
                case 4:
                    sprite->position.x += spread;
                    sprite->position.z -= spread;
                    break;
            }
            sprite->position.x += effect->shakeX;
            sprite->position.z += effect->shakeZ;
            i++;
        } while (i <= 4);
    }
    return 0;
}

s32 StagedEffect_Destroy(struct StagedEffect *effect)
{
    struct DisplaySequenceSprite *sprite = effect->sprites;
    s32 i = 4;
    do {
        Sprite_Remove(sprite);
        sprite++;
        i--;
    } while (i >= 0);
    if (effect->hasScript)
        SceneMessages_UnregisterReceiver(&effect->receiver);
    return 0;
}

u32 StagedEffect_GetPaletteIndex(void)
{
    return 8;
}

u32 StagedEffect_GetPaletteIndex(void);

s32 StagedEffect_Initialize(struct StagedEffect *effect, u32 script)
{
    s32 hide;
    s32 i;
    u16 *paletteIndex;
    s16 *travel;
    u16 *savedPaletteIndex;
    s16 *savedTravel;
    u32 hasScript = Script_SeekToKeyword(0x4F);
    if (hasScript)
        effect->hasScript = Script_GetValue();
    else
        effect->hasScript = hasScript;
    if (effect->hasScript) {
        SceneMessages_RegisterReceiver(&effect->receiver, script, 10);
        {
            u32 state = 0;
            u16 *destination = &effect->state;
            *destination = state;
        }
        effect->frame = 0;
        hide = 1;
    } else {
        u32 state = 1;
        u16 *destination = &effect->state;
        *destination = state;
        effect->frame = 0;
        hide = 0;
        Sound_PlaySong(0x1C6);
    }
    if (Script_SeekToKeyword(0x70)) {
        effect->position.axes.x = Script_GetValue();
        effect->position.axes.y = Script_GetValue();
        effect->position.axes.z = Script_GetValue();
    } else {
        struct GameState *game = gGameState;
        effect->position.axes.x = (u16)game->positionX;
        effect->position.axes.y = (u16)game->positionY;
        effect->position.axes.z = (u16)game->positionZ;
    }
    {
        u32 value = StagedEffect_GetPaletteIndex();
        paletteIndex = &effect->paletteIndex;
        *paletteIndex = value;
    }
    {
        u32 value = Script_GetKeywordValue(0x68, 1024);
        travel = &effect->travel;
        *travel = value;
    }
    effect->initialTravel = *travel;
    effect->spread = Script_GetKeywordValue(0x6D, 512);
    effect->initialSpread = effect->spread;
    effect->scale = 2032;
    {
        u32 found = Script_SeekToKeyword(0x65);
        savedPaletteIndex = paletteIndex;
        savedTravel = travel;
        if (found) {
            effect->pendingScriptId = Script_GetValue();
            effect->scriptReleaseFrame = Script_GetValue();
        } else {
            effect->pendingScriptId = 0;
            effect->scriptReleaseFrame = 0;
        }
    }
    AnimData_Load(&effect->graphics, 0x2499);
    i = 0;
    do {
        struct DisplaySequenceSprite *sprite = &effect->sprites[i];
        u32 xy, z;
        Sprite_Init(sprite, &effect->graphics, 2);
        sprite->frame = *savedPaletteIndex * 7;
        xy = effect->position.words[0];
        z = effect->position.words[1];
        *(u32 *)&sprite->position.x = xy;
        *(u32 *)&sprite->position.z = z;
        if (i == 0) {
            sprite->position.y += *savedTravel;
            sprite->flags |= 1;
        } else {
            sprite->flags |= 0x200 | hide;
        }
        sprite->priority = 1;
        i++;
    } while (i <= 4);
    CpuSet((u16 *)gPaletteBankData + 0x28D0, effect->blendedPalette, 0x4000008);
    effect->activePalette = effect->blendedPalette;
    effect->graphics.palette = effect->blendedPalette;
    return 0;
}

s32 StagedEffect_Initialize(struct StagedEffect *, u32);

s32 StagedEffect_Tick(struct StagedEffect *);

s32 StagedEffect_Destroy(struct StagedEffect *);

struct StagedEffect *StagedEffect_CreateFromScript(u32 script)
{
    struct StagedEffect *effect = Task_Create(8, sizeof(struct StagedEffect));
    if (effect) {
        Task_SetCallbacks(effect, (void (*)(void *))StagedEffect_Tick,
                          (void (*)(void *))StagedEffect_Destroy);
        if (StagedEffect_Initialize(effect, script) < 0) {
            Task_Kill(effect);
            return 0;
        }
    }
    return effect;
}
