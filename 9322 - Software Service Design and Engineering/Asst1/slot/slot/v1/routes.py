# -*- coding: utf-8 -*-

###
### DO NOT CHANGE THIS FILE
### 
### The code is auto generated, your change will be overwritten by 
### code generating.
###
from __future__ import absolute_import

from .api.alltime import Alltime
from .api.time_id import TimeId
from .api.available import Available
from .api.book import Book
from .api.cancel import Cancel


routes = [
    dict(resource=Alltime, urls=['/alltime'], endpoint='alltime'),
    dict(resource=TimeId, urls=['/time/<int:id>'], endpoint='time_id'),
    dict(resource=Available, urls=['/available'], endpoint='available'),
    dict(resource=Book, urls=['/book'], endpoint='book'),
    dict(resource=Cancel, urls=['/cancel'], endpoint='cancel'),
]