# Actor_InitializeController

All 576 bytes of the initializer match. Each of the sixteen child sprites
receives its index, the idle callback and its initial control bits. Grouping the
last three stores in a callback-first helper gives the hoisted callback address
r2 before its move into the saved register. The idle callback now accepts the
same controller and child arguments as the slot that holds it.
