# -*- coding: utf-8 -*-
from __future__ import absolute_import, print_function

from flask import request, g

from . import Resource
from .. import schemas
from .. import data
import copy

def time2string(time):
    if time == 0:
        return "9 AM"
    if time == 1:
        return "10 AM"
    if time == 2:
        return "11 AM"
    if time == 3:
        return "12 AM"
    if time == 4:
        return "1 PM"
    if time == 5:
        return "2 PM"
    if time == 6:
        return "3 PM"
    if time == 7:
        return "4 PM"
    return "Error time"


class Available(Resource):

    def get(self):
        info = copy.deepcopy(data.data)
        # result = []
        # for doctor in info:
        #     doctor["timetable"] = []
        #     for (i, avl) in enumerate(doctor['available']):
        #         table = {}
        #         table["time"] = time2string(i)
        #         table["available"] = avl
        #         doctor["timetable"].append(table)
        #     del doctor["available"]
        #     # print(doctor)
        #     result.append(doctor)
        # print(result)
        return info, 200, None