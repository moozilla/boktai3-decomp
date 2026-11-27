"""Ownership evidence for shared inline helper replacements."""
from pathlib import Path
import sys
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))

import inline_twins


class HelperOwnershipTests(unittest.TestCase):
    def test_shared_body_alone_cannot_introduce_an_unrelated_object_dependency(self):
        source = "static inline int Step(int value) { return value + 1; }\n"
        private, = inline_twins.helpers(source)
        source_path = "src/monsters/centipede/Centipede_Update.c"
        unrelated, = inline_twins.helpers(source.replace("Step", "Golem_Step"), "monsters/golem/golem.h")
        common, = inline_twins.helpers(source.replace("Step", "Scene_Step"), "scene/actor/step.h")
        local, = inline_twins.helpers(source.replace("Step", "Centipede_Step"), "monsters/centipede/step.h")
        exact, = inline_twins.helpers(source, "bosses/ratatosk/step.h")
        with patch.object(inline_twins, "visible", return_value=False):
            self.assertEqual(inline_twins.related_helpers(source_path, private, [unrelated, common, local, exact]),
                             [common, local, exact])
        with patch.object(inline_twins, "visible", return_value=True):
            self.assertEqual(inline_twins.related_helpers(source_path, private, [unrelated]), [unrelated])


if __name__ == "__main__":
    unittest.main()
