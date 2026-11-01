# Reaction motion mode (08080BE0)

Tags: #matching #scene-actors

SceneActorReaction_SetMotionMode matches all 980 bytes without pins. The
callback clears full-width control flags, preserves the masked entry bit when
combining the new motion flags, and tests the value it just stored. The signed
halfword mask in the hit-state branch produces the same full-width ~0x201
literal as the sibling damage handlers.
