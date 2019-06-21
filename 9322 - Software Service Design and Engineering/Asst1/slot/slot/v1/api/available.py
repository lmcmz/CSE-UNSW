# -*- coding: utf-8 -*-
from __future__ import absolute_import, print_function

from flask import request, g

from . import Resource
from .. import schemas
from .. import data
import requests, json

class Available(Resource):

    def post(self):
        print(request.form)
        timeEnum = int(request.form["time"])
        day = request.form["day"]
        if timeEnum < 0 or timeEnum >= 8:
            return {'code': 2, 'msg': "Time is out of range"}, 400, None
        
        available_list = []
        for doctor in data.data:
            if doctor['weekdays'][day][timeEnum] == False:
                continue
            available_list.append(doctor["id"])

        result = []
        for doctorID in available_list:
            r = requests.get("http://0.0.0.0:5000/v1/avilable/{}".format(doctorID))
            jsonData = json.loads(r.text)
            result.append(jsonData)
        return result, 200, None