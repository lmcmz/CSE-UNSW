# -*- coding: utf-8 -*-

###
### DO NOT CHANGE THIS FILE
### 
### The code is auto generated, your change will be overwritten by 
### code generating.
###
from __future__ import absolute_import

from .api.dentist import Dentist
from .api.available import Available
from .api.avilable_id import AvilableId
from .api.book import Book
from .api.cancel import Cancel


routes = [
    dict(resource=Dentist, urls=['/dentist'], endpoint='dentist'),
    dict(resource=Available, urls=['/available'], endpoint='available'),
    dict(resource=AvilableId, urls=['/avilable/<int:id>'], endpoint='avilable_id'),
    dict(resource=Book, urls=['/book'], endpoint='book'),
    dict(resource=Cancel, urls=['/cancel'], endpoint='cancel'),
]