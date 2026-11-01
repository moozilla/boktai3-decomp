# Collision_ProcessPairs

The 288-byte function matches in ordinary C. Both collider lists go to
PositionList_UpdateTranslatedCoordinates. Calling Box_Overlaps through the typed
predicate leaves r2 free for the callback-suppression mask. Equal nonzero
collision IDs suppress a pair, and reward flag bit 21 chooses callback order.
