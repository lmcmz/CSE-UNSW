#!/usr/bin/python3

import os, sys, traceback

try:
    from wsgiref.handlers import CGIHandler
    from werkzeug.debug import DebuggedApplication

    from UNSWtalk import app

    if 'PATH_INFO' not in os.environ:
        os.environ['PATH_INFO'] = ''

    # run the Flask app in debug mode
    app.secret_key = 'correct horse battery staple'
    app.debug = True
    CGIHandler().run(DebuggedApplication(app))
except Exception:
    # catch any exceptions that escape Flask and print useful information
    print('Content-Type: text/plain\n', flush=True)
    etype, evalue, etraceback = sys.exc_info()
    print("\n".join(traceback.format_exception_only(etype, evalue)), flush=True)
    traceback.print_exc(file=sys.stdout)