# -*- coding: utf-8 -*-
from __future__ import absolute_import, print_function

from flask import request, g

from . import Resource
from .. import schemas
from .. import data

class Alltime(Resource):

    def get(self):
        return data.data, 200, None