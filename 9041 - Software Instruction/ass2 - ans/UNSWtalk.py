#!/usr/bin/python3

import os,sys
from flask import Flask, render_template, session,request,redirect,url_for,g
import re,collections,subprocess
from collections import defaultdict
from collections import OrderedDict
from datetime import datetime

students_dir = "dataset-medium";
enc = sys.getdefaultencoding()

app = Flask(__name__)

class AutoVivification(dict):
    def __getitem__(self, item):
        try:
            return dict.__getitem__(self, item)
        except KeyError:
            value = self[item] = type(self)()
            return value

def get_dict_details(details):
    dict_student = dict()
    for line in details.split('\n'):
        if ':' not in line:
            continue
        if 'friends' in line:
            line = line.replace("(","")
            line = line.replace(")","")
            info = line.strip().split(':')
            dict_student[info[0].strip()] = [friend.strip() for friend in info[1].split(',')]
            continue
        if 'courses' in line:
            line = line.replace("(","")
            line = line.replace(")","")
            info = line.strip().split(':')
            dict_student[info[0].strip()] = [course.strip().split(' ')[2] for course in info[1].split(',')]
            continue
        info = line.strip().split(':',1)
        dict_student[info[0].strip()] = info[1].strip()
    return dict_student

def get_all_students_info():
    all_info_init = dict()
    students = sorted(os.listdir(students_dir))
    for stud in students:
        pattern = re.compile("^z[0-9]{7}$")
        if not pattern.match(stud): continue
        details_filename = os.path.join(students_dir, stud, "student.txt")
        with open(details_filename) as f:
            details = f.read()
        all_info_init[stud] = get_dict_details(details)
    return all_info_init

def get_all_post():
    all_messages_dict = AutoVivification()
    zid_list = list(all_info.keys())
    for zid in zid_list:
        messages = sorted(os.listdir(students_dir+'/'+zid))
        for msg in messages:
            if re.match('^\d+\.txt$', msg):
                details_filename = os.path.join(students_dir, zid, msg)
                with open(details_filename,encoding = enc) as f:
                    details = f.read()
                msg = msg.replace(".txt","")
                all_messages_dict[zid][msg] = get_dict_details(details)
    return all_messages_dict
    
all_info = get_all_students_info()
all_messages = get_all_post()

@app.route('/', methods=['GET'])
def init():
    return render_template('login.html',message='') 
    
@app.route('/logout', methods=['GET'])
def logout():
    session['userID'] = ""
    g.current_user_id = ""
    return render_template('login.html',message='') 

@app.route('/register', methods=['GET', 'POST'])
def register():
    return render_template('register.html') 

@app.route('/login', methods=['GET','POST'])
def login():
    get_all_post()
    zid = request.form.get('zid', '')
    password = request.form.get('password', '')
    #zid = "z5190009"
    #password = "xxxxxxxx"
    #print("zid"+zid)
    if zid	not in all_info:
        return render_template('login.html', message='Account not exists')
    if password != all_info[zid]['password']:
        return render_template('login.html', message='Password is incorrect')
    
    session['userID'] = zid
    g.current_user_id = zid
    
    return redirect( url_for('home'))
                        
@app.route('/home', methods=['GET','POST'])
def home():
    zid = session.get('userID', '')
    if len(zid) == 0:
        zid = g.current_user_id
    if len(zid) == 0:
        return redirect(url_for('init'))
        
    dict_details = all_info[zid]
    messages = sorted(os.listdir(students_dir+'/'+zid))
    all_message = get_student_messages(zid,messages)
    
    if len(dict_details['home_suburb']) > 0:
        dict_details['home_suburb'] = "unknow"
    
    return render_template('profile.html', 
                        student_details=dict_details,
                        student_zid=zid,
                        student_name=dict_details['full_name'],
                        student_location=dict_details['home_suburb'],
                        student_email=dict_details['email'],
                        student_DOB=dict_details['birthday'],
                        student_program=dict_details['program'],
                        student_course=',\n'.join(dict_details['courses']),
                        student_friends=dict_details['friends'],
                        student_messages=all_message,
                        all_students_info=all_info)


@app.route('/visit/<zid>', methods=['GET','POST'])
def visit(zid):
    user_id = session.get("userID",'')
    if zid == user_id:
        return redirect(url_for('home'))
    dict_details = all_info[zid]
    messages = sorted(os.listdir(students_dir+'/'+zid))
    all_message = get_student_messages(zid,messages)
    
    if 'home_suburb' not in dict_details:
        dict_details['home_suburb'] = "unknow"
    
    return render_template('visit.html', 
                        student_details=dict_details,
                        student_zid=zid,
                        student_name=dict_details['full_name'],
                        student_location=dict_details['home_suburb'],
                        student_email=dict_details['email'],
                        student_DOB=dict_details['birthday'],
                        student_program=dict_details['program'],
                        student_course=',\n'.join(dict_details['courses']),
                        student_friends=dict_details['friends'],
                        is_Friend=is_friend(user_id, zid),
                        student_messages=all_message,
                        all_students_info=all_info)

@app.route('/search', methods=['GET','POST'])
def search():
    keywords = request.form.get('search', '')
    stu_matchList = list()
    post_matchList = AutoVivification()
    for zid,subDict in all_info.items():
        if keywords.lower() in subDict['full_name'].lower():
            stu_matchList.append(zid)
    for zid,subDict in all_messages.items():
        for key,value in subDict.items():
            if keywords.lower() in value['message']:
                post_matchList[zid] = value
    #print(post_matchList)
    return render_template('search.html', 
                                        match_students=stu_matchList,
                                        all_students_info=all_info,
                                        student_messages=post_matchList)

@app.route('/post', methods=['GET','POST'])
def post():
    zid = session.get("userID")
    if len(zid) == 0:
        zid = g.current_user_id
    message = request.form.get('message', '')
    time = datetime.now().strftime('%Y-%m-%dT%H:%M:%S+0000')
    content = "from: " + zid + "\n" +"time: " + time + "\n"+ "message: " + message
    
    messages = sorted(os.listdir(students_dir+'/'+zid))
    make_student_post(zid,content,messages)
    return redirect(url_for('home'))
    
@app.route('/follow', methods=['GET','POST'])
def follow():
    user_id = session.get("userID")
    if len(user_id) == 0:
        user_id = g.current_user_id
    friend_id = request.form.get('zid', '')
    follow_user(user_id,friend_id)
    return redirect(url_for('visit', zid=friend_id))
    
@app.route('/unfollow', methods=['GET','POST'])
def unfollow():
    user_id = session.get("userID")
    if len(user_id) == 0:
        user_id = g.current_user_id
    friend_id = request.form.get('zid', '')
    unfollow_user(user_id,friend_id)
    return redirect(url_for('visit', zid=friend_id))

    
@app.route('/reset', methods=['GET','POST'])
def reset():
    return render_template('reset.html')
    
@app.route('/create', methods=['GET','POST'])
def create():
    return redirect(url_for('login'))


def get_student_info_with_zid(zid,key):
    details_filename = os.path.join(students_dir, zid, "student.txt")
    with open(details_filename) as f:
        details = f.read()
    dict_details = get_dict_details(details)
    return dict_details[key]

def get_student_messages(zid,messages):
    number = 0
    dict_details = AutoVivification()
    #print(messages)
    for msg in messages:
        if '.txt' not in msg or msg == 'student.txt':
            continue
        details_filename = os.path.join(students_dir, zid, msg)
        with open(details_filename,encoding = enc) as f:
            details = f.read()
        msg = msg.replace(".txt","")
        msg_list = msg.split('-')
        number +=1
        if len(msg_list) == 1:
            if str(msg_list[0]) in dict_details:
                dict_details[str(msg_list[0])].update(get_dict_details(details))
            else:
                dict_details[str(msg_list[0])] = get_dict_details(details)
        if len(msg_list) == 2:
            dict_details[str(msg_list[0])] = dict_details.get(str(msg_list[0]), {})
            dict_details[str(msg_list[0])][str(msg_list[1])] = get_dict_details(details)
        if len(msg_list) == 3:
            dict_details[str(msg_list[0])] = dict_details.get(str(msg_list[0]), {})
            dict_details[str(msg_list[0])][str(msg_list[1])] = dict_details[str(msg_list[0])].get(str(msg_list[1]), {})
            dict_details[str(msg_list[0])][str(msg_list[1])][str(msg_list[2])] = get_dict_details(details)
    temp = reversed( sorted(dict_details.items(), key=lambda x: int(x[0])) )
    new_dict = OrderedDict((x, y) for x, y in temp)
    #print(new_dict)
    return new_dict
    
def make_student_post(zid,content,messages):
    post_number = -1;
    for msg in messages:
        msg = msg.replace(".txt","")
        if re.match('^\d+$', msg):
            if post_number < int(msg):
                post_number = int(msg)
    post_number += 1
    post_file_name = str(post_number) + ".txt"
    details_filename = os.path.join(students_dir, zid, post_file_name)
    with open(details_filename, 'w') as f:
        f.write(content)
    f.close()
    
def follow_user(user_id,friend_id):
    friend_list = list(all_info[user_id]['friends'])
    friend_list.insert(0, friend_id)
    all_info[user_id]['friends'] = friend_list
    friend_string = ', '.join(friend_list)
    details_filename = os.path.join(students_dir, user_id, 'student.txt')
    with open(details_filename, 'r+',encoding = enc) as f:
        content = ""
        for line in f.readlines():
            if 'friends' in line:
                content += "friends: (" + friend_string + ")\n"
            else:
                content += line
        f.seek(0)
        f.truncate()
        f.write(content)
        f.close()

def unfollow_user(user_id,friend_id):
    friend_list = list(all_info[user_id]['friends'])
    friend_list.remove(friend_id)
    all_info[user_id]['friends'] = friend_list
    friend_string = ', '.join(friend_list)
    details_filename = os.path.join(students_dir, user_id, 'student.txt')
    with open(details_filename, 'r+',encoding = enc) as f:
        content = ""
        for line in f.readlines():
            if 'friends' in line:
                content += "friends: (" + friend_string + ")\n"
            else:
                content += line
        f.seek(0)
        f.truncate()
        f.write(content)
        f.close()
    
def is_friend(user_id,friend_id):
    friend_list = list(all_info[user_id]['friends'])
    return friend_id in friend_list

def send_email(to, subject, message):
    mutt = [
            'mutt',
            '-s',
            subject,
            '-e', 'set copy=no',
            '-e', 'set realname=UNSWtalk',
            '--', to
    ]
    subprocess.run(
            mutt,
            input = message.encode('utf8'),
            stderr = subprocess.PIPE,
            stdout = subprocess.PIPE,
    )
        
if __name__ == '__main__':
    app.secret_key = os.urandom(12)
    app.run(debug=True)
