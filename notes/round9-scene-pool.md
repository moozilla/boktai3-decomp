# Scene pool

Tags: 0803D288 #matching #scene-pool

`ScenePool_SpawnEntriesNearPlayers` matches all 424 bytes without register pins.
Indexing the two bound arrays with the same player counter lets agbcc allocate
both loops. Taking the region and bucket-count addresses before loading either
value reproduces the hash call's argument order.
