# -*- coding: utf-8 -*-
from __future__ import absolute_import, print_function

from flask import request, g

import requests

from . import Resource
from .. import schemas
from .. import data

class AvilableId(Resource):

    def get(self, id):
        for doctor in data.data:
            if doctor['id'] == id:
                requests.get("http://0.0.0.0:5001/")
                return doctor, 200, None
        return {'msg': 'Can not found dentist id'}, 404, None