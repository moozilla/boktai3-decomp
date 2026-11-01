# Registered actor terrain reactions

Tags: #matching #scene-actors 08072854

SceneActorRegistry_ApplyTerrainReactions matches all 752 bytes. On sinking
terrain, the actor installs either its own sink control or the default
SceneActorReaction_SinkAndRemove. Installing the control in each branch
preserves the original shared tail. Byte parameters in the state setter
leave the zero initialization after the first field address is formed.
