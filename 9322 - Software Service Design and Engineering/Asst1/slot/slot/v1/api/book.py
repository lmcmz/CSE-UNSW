# -*- coding: utf-8 -*-
from __future__ import absolute_import, print_function

from flask import request, g

from . import Resource
from .. import schemas
from .. import data
import random

def time2string(time):
    if time == 0:
        return "9AM"
    if time == 1:
        return "10AM"
    if time == 2:
        return "11AM"
    if time == 3:
        return "12AM"
    if time == 4:
        return "1PM"
    if time == 5:
        return "2PM"
    if time == 6:
        return "3PM"
    if time == 7:
        return "4 PM"
    return "Error time"

def suggest(doctorID):
    for doctor in data.data:
        if doctor['id'] == doctorID:
            day = random.choice(["monday", "tuesday", "wednesday", "thursday", "friday"])
            time = random.choice([0,1,2,3,4,5,6,7])
            if doctor['weekdays'][day][time] == False:
                return suggest(doctorID)
            return (day, time2string(time))

class Book(Resource):

    def post(self):
        print(request.form)
        doctorID = int(request.form["id"])
        timeEnum = int(request.form["time"])
        day = request.form["day"]
        if timeEnum < 0 or timeEnum >= 8:
            return {'code': 2, 'msg': "Time is out of range"}, 400, None
        for doctor in data.data:
            if doctor['id'] == doctorID:
                if doctor['weekdays'][day][timeEnum] == False:
                    day, time = suggest(doctorID)
                    return {'code': 3, 'msg': "Already Booked, but i can book at {} on {}".format(time, day)}, 200, None
                doctor['weekdays'][day][timeEnum]  = False
                data.writeData(data.data)
                # print(data.data)
                return {'code': 0, 'msg': "Book success"}, 200, None
        return {'code': 1, 'msg': "error"}, 200, None