# -*- coding: utf-8 -*-
from __future__ import absolute_import, print_function

from flask import request, g

from . import Resource
from .. import schemas
from .. import data

class TimeId(Resource):

    def get(self, id):
        for doc in data.data:
            if id == doc["id"]:
                return doc,200,None
        return None, 200, None