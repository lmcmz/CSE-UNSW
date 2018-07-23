from flask import Flask,jsonify, url_for, Response, render_template, make_response
from flask_restful import reqparse, request
import time, json
from src.json2xml import Json2xml
from bson.json_util import loads
from bson import Binary, Code
from bson.json_util import dumps, CANONICAL_JSON_OPTIONS
from functools import wraps
import base64
import urllib.parse as urlparse
from urllib.parse import urlencode

import getPostcode, DB, bnf2db

app = Flask(__name__)
formatStr = "format"
lgaNameStr = "lgaName"
postcodeStr = "postcode"
filterStr = "filter"
lgaStr = "lga"


################################### Auth ###################################

def check_auth(username, password):
	
	##### BASE64 decoding
	username = base64.b64decode(username)
	password = base64.b64decode(password)
	username = bytes.decode(username)
	password = bytes.decode(password)
	
	if username == 'admin' and password == 'admin':
		return True
	if username == 'guest':
		return True
	return False
		

def authenticate():
	return Response(
	'Could not verify your access level for that URL.\n'
	'You have to login with proper credentials', 401,
	{'WWW-Authenticate': 'Basic realm="Login Required"'})

def requires_auth(f):
	@wraps(f)
	def decorated(*args, **kwargs):
		auth = request.authorization
		if not auth or not check_auth(auth.username, auth.password):
			return authenticate()
		return f(*args, **kwargs)
	return decorated

################################### Utils ###################################

def makeReturnDict(suburbList, collection):
	new_dict = {}
	for sub in suburbList:
		timeStr = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.localtime()) 
		new_dict[sub] = {}
		new_dict[sub]['name'] = 'admin'
		new_dict[sub]['titile'] = 'Create data success'
		new_dict[sub]['collection'] = collection
		new_dict[sub]['id'] = sub
		new_dict[sub]['time'] = timeStr
		new_dict[sub]['URI'] = "/"+collection+"/"+sub
	return new_dict
	
def reponse_format(response, format, isATOM):
	response.headers['Content-Type'] = 'application/xml'
	if format == 'json':
		response.headers['Content-Type'] = 'application/json'
		return response
	data = loads(response.data)
	data_object = Json2xml(data)
	xml = data_object.json2xml()
	response.data = xml
	return response

def paramHandle(params):
	if not lgaStr in params:
		return None
	
	lga = params[lgaStr]
	suburbList = getPostcode.findSuburb(lga)
	if len(suburbList) == 0:
		return []
	fetchList = []
	for sub in suburbList:
		if getPostcode.is_downloadable_suburb(sub):
			fetchList.append(sub)
	return fetchList
	

################################### Flask ###################################

########## Create Data ######### 

@app.route("/<collection>", methods=['POST'])
@requires_auth
def create_lga(collection):
	params = request.form
	format = params.get('format', 'xml')
	if not lgaStr in params:
		response = jsonify(error="Empty input")
		return reponse_format(response ,format, True),400
	
	suburbList = paramHandle(params)
	if not suburbList:
		response = jsonify(error="Invaild input, can't found suburb")
		return reponse_format(response ,format, True), 404
	
	isNewCreate = True
	fetch_list, exist_list = DB.checkListExistInDB(suburbList, collection)
	
	if not len(fetch_list) == 0:
		for sub in fetch_list:
			path = getPostcode.getLGAData(sub)
			DB.storeXlSXToDB(path,collection)
	else:
		isNewCreate = False
	
	returnDict = makeReturnDict(suburbList, collection)
	code = 201 if isNewCreate else 200
	if format == "xml":
		return render_template('create_collection.xml', suburbs=returnDict),code
	return json.dumps(returnDict,indent=4),code

########## GET Data ######### 

@app.route("/<collection>", methods=['GET'])
@requires_auth
def get_lga(collection):
	params = request.args
	format = params.get('format','xml')
	if not lgaStr in params and not filterStr in params:
		response = jsonify(error="Empty input")
		return reponse_format(response ,format, True),400
	
	if filterStr in params:
		queryStr,queryList = bnf2db.covert2DB(params[filterStr])
		result = DB.getDataByFilter(queryStr, queryList,collection)
		if not result:
			response = jsonify(error="Can't find it in DB")
			return reponse_format(response ,format, True),404
		result = dumps(result, indent=4)
		response = Response()
		response.data = result
		return reponse_format(response ,format, True),200

	suburbList = paramHandle(params)
	if not suburbList:
		response = jsonify(error="Invaild input, can't found suburb")
		return reponse_format(response ,format, True), 404
	
	fetch_list, exist_list = DB.checkListExistInDB(suburbList, collection)
	if len(exist_list) == 0:
		return jsonify(error="Can't find suburb info in DB"), 404
	
	data = DB.getListFile(collection, exist_list)
	data_object = Json2xml(data)
	jsonData = dumps(data, indent=4)
	xmlData = data_object.json2xml()
	formatData = xmlData
	if format == "json":
		formatData = jsonData
	return formatData
	
@app.route("/<collection>/<suburb>", methods=['GET'])
#@requires_auth
def get_single_lga(collection, suburb):
	params = request.args
	format = params.get('format','json')
	if not getPostcode.is_downloadable_suburb(suburb):
		response = jsonify(error="Invaild input")
		return reponse_format(response ,format, True),400
	
	data = DB.getFile(collection, suburb)
	if not data:
		return jsonify(error="Can't find " + suburb + " info in DB"), 404
	jsonData = dumps(data, indent=4)
	data_object = Json2xml(data)
	xmlData = data_object.json2xml()
	formatData = xmlData
	if format == "json":
		formatData = jsonData
	return formatData

@app.route("/<collection>/all", methods=['GET'])
@requires_auth
def get_all_lga(collection):
	params = request.form
	format = params.get('format', 'xml')
	data = DB.getAllFile(collection)
	data_object = Json2xml(data)
	jsonData = dumps(data, indent=4)
	xmlData = data_object.json2xml()
	formatData = xmlData
	if format == "json":
		formatData = jsonData
	return formatData

########## Delete Data ######### 

@app.route("/<collection>", methods=['DELETE'])
@requires_auth
def delete_lga(collection):
	params = request.form
	format = params.get('format', 'xml')
	if not lgaStr in params:
		response = jsonify(error="Empty input")
		return reponse_format(response ,format, True),400
		
	suburbList = paramHandle(params)
	if not suburbList:
		response = jsonify(error="Invaild input, can't found suburb")
		return reponse_format(response ,format, True), 404

	if len(suburbList) > 1:
		response = jsonify(error="Multiple suburb have same postcode. Please delete by name", suburbList=suburbList)
		return reponse_format(response ,format, True), 200
	
	suburb = suburbList[0]
	if not DB.checkExistInDB(suburb, collection):
		response = jsonify(error="Invaild input, can't found suburb")
		return reponse_format(response ,format, True), 404
	
	bool, result = DB.deleteFile(collection, suburb)
	result = dumps(result, indent=4)
	response = jsonify(DeleteSuccess=bool)
	return reponse_format(response ,format, True), 200
	

################################### Main ###################################

if __name__ == "__main__":
	app.config['JSON_AS_ASCII'] = False
	app.run(debug=True, port=5002)
	
