import unittest
from tempfile import TemporaryDirectory
from unittest.mock import Mock, patch
from pathlib import Path

from tools.scope import fast


class PrefixServers(unittest.TestCase):
    def test_bounded_prefixes_close_the_least_recently_used_compiler(self):
        with TemporaryDirectory() as directory:
            compiler = Path(directory) / "agbcc"
            compiler.touch()
            servers = [Mock() for _ in range(4)]
            for server in servers:
                server.proc.poll.return_value = None
            with patch.dict(fast.os.environ, SCOPE_PREFIX_SERVERS="2"), patch.object(
                fast, "Server", side_effect=servers
            ):
                cache = fast.Fast(str(compiler), [], [], {}, 0, directory)
                first = cache.server("a")
                second = cache.server("b")
                self.assertIs(cache.server("a"), first)
                third = cache.server("c")
                second.close.assert_called_once()
                first.close.assert_not_called()
                first.proc.poll.return_value = 1
                replacement = cache.server("a")
                first.close.assert_called_once()
                self.assertIsNot(replacement, first)
                self.assertIs(cache.server("c"), third)
                cache.close()
                third.close.assert_called_once()
                replacement.close.assert_called_once()
