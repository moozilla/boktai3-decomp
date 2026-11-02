// MATCHING_CONCESSION: register-pinning
#include "display_track.h"
void Anim_SetDurationScale(void *, u32);
void DisplayTrack_AddFromScript(void)
{
    struct DisplayTrackManager *manager = gDisplayTrackManager;
    struct DisplayTrack *track;
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
            u32 event;
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
            DisplayTrack_StepSequence(track, &track->sprite, state);
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
