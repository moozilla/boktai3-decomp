# Contact target and damage update

Tags: #scene-actors #matching 0807E9D0

The guarded callback now uses the source and destination collider layouts and the
registered actor's tracking fields. The contact flag paths each select their target
and the game reads the received power as an unsigned halfword before halving it.
The three-particle burst shares its RNG indexing and vector layout with the registry
and hit-reaction paths. The default build keeps the assembly body. The target selection
branches and their retained field pointers remain the largest differences.
