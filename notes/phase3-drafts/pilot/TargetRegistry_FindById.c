/* Draft A (registers right, cross-jump direction wrong): */
struct MotionTarget *TargetRegistry_FindById(u32 id)
{
    struct MotionTargets *registry = gSceneLinkList;
    struct MotionTarget *entry;
    if (registry) {
        entry = registry->first;
        if (entry) do {
            u32 flags = entry->flags;
            u32 sceneActor = flags & 0x10;
            u32 disabled = flags & 1;
            if (!sceneActor)
                goto actor;
            if (entry->target.collider->collisionId != id)
                continue;
            if (!disabled)
                return entry;
            break;
        actor:
            if (entry->target.actor->id != id)
                continue;
            if (!disabled)
                return entry;
            break;
        } while (entry);
    }
    return NULL;
}

/* Draft B (layout right, id/sceneActor r4/r3 swapped): */
struct MotionTarget *TargetRegistry_FindById_B(u32 id)
{
    struct MotionTargets *registry = gSceneLinkList;
    struct MotionTarget *entry;
    if (registry) {
        entry = registry->first;
        /* The original never advances to entry->next: an ID mismatch tests the same entry again. */
        if (entry) do {
            u32 flags = entry->flags;
            u32 sceneActor = flags & 0x10;
            u32 disabled = flags & 1;
            u32 entryId;
            if (!sceneActor)
                goto actor;
            entryId = entry->target.collider->collisionId;
        compare:
            if (entryId != id)
                continue;
            if (!disabled)
                return entry;
            break;
        actor:
            entryId = entry->target.actor->id;
            goto compare;
        } while (entry);
    }
    return NULL;
}
