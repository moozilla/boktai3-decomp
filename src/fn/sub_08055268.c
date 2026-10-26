// MATCHING_CONCESSION: register-pinning
#include "display_track.h"
void Anim_SetDurationScale(void *, u32);
void DisplayTrack_AddFromScript(void)
{
    struct DisplayTrackManager *manager = gDisplayTrackManager;
    struct DisplayTrack *track;
    struct ContactSwarmSprite *savedSprite;
    struct DisplayTrackGraphics *graphics;
    register u32 value asm("r0");
    u32 resource;
    u32 frame;
    u32 shadow;
    if (!manager || manager->count >= manager->capacity)
        return;
    track = &manager->entries[manager->count];
    value = Script_SeekToKeyword('n');
    if (value)
        value = Script_GetValue();
    track->tag = value;
    {
        struct ContactSwarmSprite *sprite = &track->sprite;
        graphics = &track->graphics;
        value = Script_SeekToKeyword('t');
        savedSprite = sprite;
        if (!value)
            return;
        resource = Script_GetValue();
        if (Script_SeekToKeyword('i'))
            frame = Script_GetValue();
        else
            frame = 0;
        AnimData_Load(graphics, resource);
        Sprite_Init(sprite, graphics, 0);
        sprite->frame = frame;
        {
            u32 palette = Script_GetKeywordValue('c', 0);
            if (palette)
                AnimData_SelectPaletteBank(graphics, palette);
        }
        track->paletteBase = graphics->defaultPalette;
        track->mirror = 0;
        if (Script_SeekToKeyword('f'))
            track->mirror = Script_GetValue();
        if (track->mirror)
            sprite->flags |= 4;
        value = Script_SeekToKeyword('p');
        if (value) {
            sprite->position.x = Script_GetValue();
            sprite->position.y = Script_GetValue();
            value = Script_GetValue();
        } else {
            register u16 zero asm("r0") = value;
            sprite->position.x = zero;
            sprite->position.y = zero;
        }
        sprite->position.z = value;
        value = Script_SeekToKeyword('A');
        if (value)
            value = Script_GetValue();
        track->runDuringPause = value;
        if (Script_SeekToKeyword('R'))
            sprite->priority = Script_GetValue();
    }
    {
        u32 hasSequence = Script_SeekToKeyword('m');
        if (hasSequence) {
            struct DisplaySequenceState *state;
            const u16 *command;
            u32 event;
            struct ContactSwarmSprite *sprite;
            track->sequenceResource = ScriptProc_Start(0x922E, Script_GetValue());
            track->sequenceId = Script_GetValue();
            track->sequenceRow = Script_GetValue();
            event = Script_SeekToKeyword('o');
            if (event)
                track->sequenceEvent = Script_GetValue();
            else {
                asm("" : "+r"(event));
                track->sequenceEvent = event;
            }
            event = track->sequenceEvent;
            if (event && (event = Script_SeekToKeyword('e')))
                track->sequenceEventValue = Script_GetValue();
            else
                track->sequenceEventValue = event;
            state = &track->sequenceState;
            Anim_SetSequence(state, (void *)track->sequenceResource, track->sequenceId,
                             track->sequenceRow, track->mirror);
            if (Script_SeekToKeyword('P'))
                Anim_SetDurationScale(state, (u16)Script_GetValue());
            sprite = savedSprite;
            command = &state->commands[state->index];
            sprite->frame = *command >> 6;
            {
                register u32 raw asm("r0") = state->flags;
                register u32 bit asm("r3");
                register u32 flags asm("r1");
                bit = 1;
                flags = bit;
                flags &= raw;
                if (flags != (((*command & 0x30) >> 4) & bit))
                    track->sprite.flags |= 4;
                else
                    track->sprite.flags &= ~4;
                raw = state->flags;
                bit = 2;
                flags = bit;
                flags = (u8)(flags & raw);
                if (flags != (((*command & 0x30) >> 4) & bit))
                    sprite->flags |= 8;
                else
                    sprite->flags &= ~8;
            }
            {
                u32 elapsed = state->elapsed + 1;
                u32 mask;
                u32 duration;
                register u32 reverseDuration asm("r3");
                state->elapsed = elapsed;
                {
                    register u32 literal asm("r1") = 0xFFFF;
                    asm("" : : "r"(literal));
                    mask = literal;
                }
                duration = state->duration;
                elapsed = (u16)elapsed;
                reverseDuration = duration;
                if (elapsed >= duration) {
                    u32 next;
                    register u32 flags asm("r1");
                    state->elapsed = 0;
                    flags = state->flags;
                    if (flags & 4) {
                        next = state->index;
                        if (next == 0) {
                            if (flags & 8) {
                                state->elapsed = reverseDuration;
                                goto frame_ready;
                            }
                            next = state->frameCount;
                        }
                        state->index = next - 1;
                    } else {
                        u32 count;
                        next = state->index + 1;
                        state->index = next;
                        count = state->frameCount;
                        if ((next & mask) >= count) {
                            u32 clamp = (u8)(flags & 8);
                            if (clamp) {
                                state->elapsed = duration;
                                clamp = count - 1;
                            }
                            state->index = clamp;
                        }
                    }
                frame_ready:
                    command = &state->commands[state->index];
                    {
                        register u32 data asm("r1") = *command;
                        state->frameWeight = data & 15;
                    }
                    {
                        s32 weight = state->frameWeight;
                        s32 speed = state->speed;
                        s32 delay = (speed * weight) >> 6;
                        state->duration = delay;
                        if (!(u8)delay)
                            state->duration = 1;
                    }
                }
            }
            track->flags = 8;
        } else {
            asm("" : "+r"(hasSequence));
            track->sequenceResource = (void *)hasSequence;
            track->sequenceEvent = hasSequence;
            track->sequenceEventValue = hasSequence;
            track->flags = hasSequence;
        }
    }
    shadow = 0;
    if (Script_SeekToKeyword('s'))
        shadow = Script_GetValue();
    track->ownedResource = 0;
    if (shadow) {
        track->ownedResource = Heap_Alloc(64);
        if (track->ownedResource) {
            Memory_Clear(track->ownedResource, 64);
            Shadow_Initialize(track->ownedResource, &track->sprite.position, 0);
        }
    }
    gDisplayTrackManager->count++;
}
