# -*- coding: utf-8 -*-
from __future__ import absolute_import, print_function

from flask import request, g

from . import Resource
from .. import schemas
from .. import data

class Cancel(Resource):

    def post(self):
        doctorID = int(request.form["id"])
        timeEnum = int(request.form["time"])
        day = request.form["day"]
        if timeEnum < 0 or timeEnum >= 8:
            return {'code': 3, 'msg': "Time is out of range"}, 400, None
        for doctor in data.data:
            if doctor['id'] == doctorID:
                if doctor['weekdays'][day][timeEnum] == True:
                    return {'code': 4, 'msg': "Have not Booked"}, 200, None
                doctor['weekdays'][day][timeEnum]  = True
                data.writeData(data.data)
                # print(data.data)
                return {'code': 0, 'msg': "Cancel success"}, 200, None
        return {'code': 5, 'msg': "error"}, 200, None