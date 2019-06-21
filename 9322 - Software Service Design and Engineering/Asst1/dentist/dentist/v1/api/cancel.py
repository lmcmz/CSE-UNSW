# -*- coding: utf-8 -*-
from __future__ import absolute_import, print_function

from flask import request, g

from . import Resource
from .. import schemas
from .. import data

class Cancel(Resource):

    def post(self):
        doctorID = g.json["id"]
        timeEnum = g.json["time"]
        if timeEnum < 0 or timeEnum >= 8:
            return {'code': 2, 'msg': "Time is out of range"}, 400, None
        for doctor in data.data:
            if doctor['id'] == doctorID:
                if doctor['available'][timeEnum] == True:
                    return {'code': 3, 'msg': "Time is not Booked"}, 200, None
                doctor['available'][timeEnum] = True
                data.writeData(data.data)
                return {'code': 0, 'msg': "Cancel success"}, 200, None

        return {'code': 1, 'msg': "Can not find doctor id"}, 404, None