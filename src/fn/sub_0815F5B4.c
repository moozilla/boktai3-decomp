#include "global.h"

// MATCHING_CONCESSION: register-pinning

struct TargetBody {
    u32 actorId;
    u16 sceneActorId;
};

struct TargetEntry {
    struct TargetBody *body;
    u32 flags;
    u32 unk_08;
    struct TargetEntry *next;
};

struct TargetRegistry {
    u8 unk_00[0x18];
    struct TargetEntry *head;
};

extern struct TargetRegistry *gUnk_02000598;

struct TargetEntry *TargetRegistry_FindById(u32 id)
{
    register u32 requestedId asm("r3") = id;
    struct TargetRegistry *registry = gUnk_02000598;
    if (registry) {
        struct TargetEntry *entry = registry->head;
        /* An ID mismatch repeats the same entry, as in the original routine. */
        if (entry) {
            do {
                u32 entryId;
                u32 flags = entry->flags;
                u32 sceneActor = flags & 0x10;
                u32 disabled = flags & 1;
                if (!sceneActor)
                    goto actorId;
                entryId = entry->body->sceneActorId;
            compareId:
                if (entryId != requestedId)
                    continue;
                if (!disabled)
                    return entry;
                break;
            actorId:
                entryId = entry->body->actorId;
                goto compareId;
            } while (entry);
        }
    }
    return NULL;
}
