"""The flight recorder's page inside the Studio: /recorder, API under /api/recorder/
(tools/recorder/server.py; sessions from build/recorder/sessions, ?session=NAME picks one)."""
import json
import os
import sys
import urllib.parse

HERE = os.path.dirname(os.path.abspath(__file__))
RECORDER = os.path.join(os.path.dirname(HERE), "recorder")


def route(handler, method):
    if method != "GET":
        return False
    raw = handler.requestline.split(" ")[1] if handler.requestline else handler.path
    u = urllib.parse.urlparse(raw)
    if u.path not in ("/recorder", "/recorder.html") and not u.path.startswith("/api/recorder/"):
        return False
    if RECORDER not in sys.path:
        sys.path.insert(0, RECORDER)
    import server  # tools/recorder/server.py
    if u.path.startswith("/api/recorder/"):
        q = {k: v[0] for k, v in urllib.parse.parse_qs(u.query).items()}
        code, body, kind = server.api(u.path[len("/api/recorder"):], q)
        if isinstance(body, (dict, list)):
            body = json.dumps(body).encode()
        handler.send(code, body, kind or "application/json")
    else:
        handler.send(200, server.page(), "text/html; charset=utf-8")
    return True
