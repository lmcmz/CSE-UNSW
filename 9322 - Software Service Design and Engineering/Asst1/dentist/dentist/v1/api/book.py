# -*- coding: utf-8 -*-
from __future__ import absolute_import, print_function

from flask import request, g

from . import Resource
from .. import schemas
from .. import data

class Book(Resource):

    def post(self):
        doctorID = g.json["id"]
        timeEnum = g.json["time"]
        if timeEnum < 0 or timeEnum >= 8:
            return {'code': 2, 'msg': "Time is out of range"}, 400, None
        for doctor in data.data:
            if doctor['id'] == doctorID:
                if doctor['available'][timeEnum] == False:
                    return {'code': 3, 'msg': "Already Booked"}, 200, None
                doctor['available'][timeEnum] = False
                data.writeData(data.data)
                # print(data.data)
                return {'code': 0, 'msg': "Book success"}, 200, None

        return {'code': 1, 'msg': "Can not find doctor id"}, 404, None