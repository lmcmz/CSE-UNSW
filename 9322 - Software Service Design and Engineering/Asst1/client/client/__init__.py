from flask import Flask, render_template, request
from flask import jsonify
from rivescript import RiveScript
from flask import render_template
import os, requests, json, re

app = Flask(__name__)

dir_path = os.path.dirname(os.path.realpath(__file__))
bot = RiveScript()
bot.load_directory(dir_path+"/brain")
bot.sort_replies()

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

def string2time(time):
    if time == "9AM":
        return 0
    if time == "10AM":
        return 1
    if time == "11AM":
        return 2
    if time == "12AM":
        return 3
    if time == "1PM":
        return 4
    if time == "2PM":
        return 5
    if time == "3PM":
        return 6
    if time == "4PM":
        return 7
    return "Error time"


def convertTime(timeslot):
    if False not in timeslot:
        return "All day available"
    result = " "
    for (i,time) in enumerate(timeslot):
        if time == True:
            result += time2string(i)
            result += " "
    return result

@app.route('/')
def hello():
    return render_template('hello.html')

@app.route('/data', methods=['POST'])
def dataFromAjax():
    sentence = request.form.get('sentence').lower()
    dialog = request.form.get('dialog')
    # Doctor info
    if sentence == "can you give me some information about doctors?" or sentence == "doctor info":
        r = requests.get("http://0.0.0.0:5000/v1/dentist")
        jsonData = json.loads(r.text)

        reply = "bot >   <br />"
        reply += "<table border=\"1\" cellspacing=\"0\"> <tbody>" 
        keys = jsonData[0].keys()
        reply += "<tr>"
        for key in keys:
            reply += "<th>"+key+"</th>"
        reply += "</tr>"

        for item in jsonData:
            reply += "<tr>"
            for key in keys:
                reply += "<td>"+str(item[key])+"</td>"
            reply += "</ tr>"
        reply +="</tbody> </table>  <br />"

        dialog = reply + dialog
        dialog = "<br />you >   "+ sentence + "<br /> " + dialog+"<br />" 
        return dialog


    # Doctor timeslot
    if sentence == "can you show me all doctor timeslot?" or sentence == "give me all doctor timeslot" or sentence == "timeslot":
        r = requests.get("http://0.0.0.0:5001/v1/alltime")
        jsonData = json.loads(r.text)

        reply = "bot >   <br />"
        reply += "<table border=\"1\" cellspacing=\"0\"> <tbody>"
        reply += "<tr>"
        reply += "<th>"+"id"+"</th>"
        reply += "<th>"+"name"+"</th>"
        reply += "<th>"+"Mon"+"</th>"
        reply += "<th>"+"Tue"+"</th>"
        reply += "<th>"+"Wed"+"</th>"
        reply += "<th>"+"Thu"+"</th>"
        reply += "<th>"+"Fri"+"</th>"
        reply += "</tr>"
        for item in jsonData:
            reply += "<tr>"
            reply += "<td>"+str(item["id"])+"</td>"
            reply += "<td>"+str(item["name"])+"</td>"
            reply += "<td>"+convertTime(item["weekdays"]["monday"])+"</td>"
            reply += "<td>"+convertTime(item["weekdays"]["tuesday"])+"</td>"
            reply += "<td>"+convertTime(item["weekdays"]["wednesday"])+"</td>"
            reply += "<td>"+convertTime(item["weekdays"]["thursday"])+"</td>"
            reply += "<td>"+convertTime(item["weekdays"]["friday"])+"</td>"
            reply += "</ tr>"
        reply +="</tbody> </table> <br />"

        dialog = reply + dialog
        dialog = "<br />you >   "+ sentence + "<br /> " + dialog+"<br />" 
        return dialog

    # Single doctor
    # can you show me doctor timeslot?
    if "can you show me doctor" in sentence:
        matchObj = re.match(r'.* show me doctor (.*) timeslot\?', sentence)
        doctorID = matchObj.group(1)
        r = requests.get("http://0.0.0.0:5001/v1/time/{}".format(doctorID))
        jsonData = json.loads(r.text)

        reply = "bot >   <br />"
        reply += "<table border=\"1\" cellspacing=\"0\"> <tbody>"
        reply += "<tr>"
        reply += "<th>"+"id"+"</th>"
        reply += "<th>"+"name"+"</th>"
        reply += "<th>"+"Mon"+"</th>"
        reply += "<th>"+"Tue"+"</th>"
        reply += "<th>"+"Wed"+"</th>"
        reply += "<th>"+"Thu"+"</th>"
        reply += "<th>"+"Fri"+"</th>"
        reply += "</tr>"

        reply += "<tr>"
        reply += "<td>"+str(jsonData["id"])+"</td>"
        reply += "<td>"+str(jsonData["name"])+"</td>"
        reply += "<td>"+convertTime(jsonData["weekdays"]["monday"])+"</td>"
        reply += "<td>"+convertTime(jsonData["weekdays"]["tuesday"])+"</td>"
        reply += "<td>"+convertTime(jsonData["weekdays"]["wednesday"])+"</td>"
        reply += "<td>"+convertTime(jsonData["weekdays"]["thursday"])+"</td>"
        reply += "<td>"+convertTime(jsonData["weekdays"]["friday"])+"</td>"
        reply += "</ tr>"

        reply +="</tbody> </table> <br />"

        dialog = reply + dialog
        dialog = "<br />you >   "+ sentence + "<br /> " + dialog+"<br />" 
        return dialog

    # Book timeslot
    # can i book doctor 0 at 10AM on monday?
    if "can i book" in sentence:
        matchObj = re.match(r'.* doctor (.*) at (.*) on (.*)\?', sentence)
        doctorID = matchObj.group(1)
        time = matchObj.group(2).upper()
        timeID = string2time(time)
        day = matchObj.group(3).lower()
        r = requests.post("http://0.0.0.0:5001/v1/book",data={'id':doctorID, 'time':timeID, 'day':day})
        jsonData = json.loads(r.text)
        reply = jsonData["msg"]

        if reply == "Book success":
            reply = "Book success! <br /> Summary: you have booked the appointment with doctor {} at {} on {}.".format(doctorID, time, day)

        dialog = "bot >   "+ reply + "<br />" + dialog +"<br />"
        dialog = "<br />you >   "+ sentence + "<br /> " + dialog+"<br />" 
        return dialog

    # Cancel timeslot
    # i would like to cancel the appointment with doctor 0 at 10AM on monday
    if "like to cancel" in sentence:
        matchObj = re.match(r'.* cancel the appointment with doctor (.*) at (.*) on (.*)', sentence)
        doctorID = matchObj.group(1)
        time = matchObj.group(2).upper()
        timeID = string2time(time)
        day = matchObj.group(3).lower()
        r = requests.post("http://0.0.0.0:5001/v1/cancel",data={'id':doctorID, 'time':timeID, 'day':day})
        jsonData = json.loads(r.text)
        reply = jsonData["msg"]

        dialog = "bot >   "+ reply + "<br />" + dialog +"<br />"
        dialog = "<br />you >   "+ sentence + "<br /> " + dialog+"<br />" 
        return dialog


    # Availabel dentist
    # which doctor are available at 2PM on friday?
    if "doctor are available" in sentence:
        matchObj = re.match(r'.* are available at (.*) on (.*)\?', sentence)
        time = matchObj.group(1).upper()
        timeID = string2time(time)
        day = matchObj.group(2).lower()
        r = requests.post("http://0.0.0.0:5001/v1/available",data={'time':timeID, 'day':day})
        jsonData = json.loads(r.text)

        
        reply = "bot >   <br />"
        reply += "<table border=\"1\" cellspacing=\"0\"> <tbody>" 
        keys = jsonData[0].keys()
        reply += "<tr>"
        for key in keys:
            reply += "<th>"+key+"</th>"
        reply += "</tr>"

        for item in jsonData:
            reply += "<tr>"
            for key in keys:
                reply += "<td>"+str(item[key])+"</td>"
            reply += "</ tr>"
        reply +="</tbody> </table>  <br />"

        dialog = reply + dialog
        dialog = "<br />you >   "+ sentence + "<br /> " + dialog+"<br />" 
        return dialog


    reply = bot.reply("localuser", sentence)
    if "ERR: No Reply Matche" in reply:
        reply = "Sorry, I don't understand."
    dialog = "bot >   "+ reply + "<br />"  + dialog +"<br />" 
    dialog = "<br/>"+"you >   "+ sentence + "<br /> " + dialog+"<br />" 
    return dialog

if __name__ == "__main__":
    app.config['JSON_AS_ASCII'] = False
    app.run('0.0.0.0', debug=True , port=5002)