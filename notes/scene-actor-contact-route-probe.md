# Contact route probe

Tags: #scene-actors #matching 0807F81C

The guarded contact probe reaches 95.2% and the original 700-byte extent. It shares
Vorn's selection and release operations. The ROM clears the old node slot only when
the release flag is set. A changed target cell triggers the mesh and obstruction tests
before the actor remembers the new cell. The remaining differences include which
tracking pointer occupies r8 and which is spilled.
