from flask import Flask,jsonify, url_for, Response, render_template, make_response, redirect
from flask_restful import reqparse, request
import requests,html,urllib.parse
import base64

app = Flask(__name__)
serviceURL = "http://127.0.0.1:5002/crime"  # server address

@app.route('/', methods=['POST','GET'])
def lga():
	oriForm = request.form.to_dict()
	form = oriForm.copy()
	form = {k: v for k, v in form.items() if v is not ''}
	if not 'method' in form:
		codeStr = "Here will show your response.\n\n Admin account: admin \n Password: admin \n\n\n Here is the account.\n\n Guest account: guest \n Password: (any)"
		return render_template('index.html' ,code=codeStr, form={'name':"admin",'password':"admin"})
	
	method = form['method']
	name = form.get('name','')
	password = form.get('password','')
	if name == '' or password == '':
		codeStr = "You need Authentication to send request.\nAccount: guest\n\nPassword: (any)"
		return render_template('index.html' ,code=codeStr, form={})
	
	##### BASE64 encoding ######### 
	name = base64.b64encode(str.encode(form['name']))
	name = bytes.decode(name)
	password = base64.b64encode(str.encode(form['password']))
	password = bytes.decode(password)
#	formatStr = form['format']
	form.pop('method')
	form.pop('name')
	form.pop('password')
	rep = requests.Response()
	isAdmin = oriForm['name'] == 'admin' and oriForm['password'] == 'admin'
	
	
	######### Fetch all data #########
	all = form.get('all','')
	if not all == '':
		rep = requests.get(serviceURL + "/all", data=form, auth=(name, password))
		codeStr = html.escape(rep.text)
		return render_template('index.html', form=oriForm, code=codeStr), 200
	
	#########  Method ######### 
	if method == 'Retrieve':
		rep = requests.get(serviceURL, params=form, auth=(name, password))
	if method == 'Create':
		if not isAdmin:
			codeStr = "You are not admin.\n\nYou can't create data\n\n"
			return render_template('index.html' ,code=codeStr, form={}) 
		rep = requests.post(serviceURL, data=form, auth=(name, password))
	if method == 'Delete':
		if not isAdmin:
			codeStr = "You are not admin.\n\n You can't delete data\n\n"
			return render_template('index.html' ,code=codeStr, form={})
		rep = requests.delete(serviceURL, data=form, auth=(name, password))
	
	######### Response ######### 
	codeStr = html.escape(rep.text)
	return render_template('index.html', form=oriForm, code=codeStr), 200

if __name__ == "__main__":
	app.config['JSON_AS_ASCII'] = False
	app.run(debug=True , port=5001)
	