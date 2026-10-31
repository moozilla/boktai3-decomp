# EffectGroup_FadeFirstActive

Tags: #effects #matching #round8

All 60 bytes at 082314C4 match. The adjacent EffectGroup_SpawnPart keeps a success flag in the caller and breaks out of the scan. Applying the same pattern lets the success edge go straight to the count decrement while the exhausted loop retains its zero test.
