# Sword script kind

Tags: #monsters #matching

Sword_InitializeFromScript matches all 212 bytes with the kind register pin removed.
The script value is stored in the eight-bit kind field before clamping. A signed clamp
with lower and upper bounds of zero and one reproduces the reload and the second store.
The byte read makes the lower-bound branch disappear during optimization.
