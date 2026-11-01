# Scene pool

Tags: 0803D288 #matching #scene-pool

`ScenePool_SpawnEntriesNearPlayers` matches all 424 bytes without register pins.
Indexing the two bound arrays with the same player counter lets agbcc allocate
both loops. Taking the region and bucket-count addresses before loading either
value reproduces the hash call's argument order.

Tags: 0803DF90 #scene-pool #nonmatching

The guarded adaptive tracker sets the entered flag on every transition. The
old random paths reached a shared store with an uninitialized value. The
new draft uses the normal entry helper throughout. Its remaining difference
is tail sharing among the calls that clear the per-state workspace.
