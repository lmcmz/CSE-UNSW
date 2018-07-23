import requests
import xlrd, pymongo, re, json, os
import xlstojson
from bson.json_util import loads
from bson import Binary, Code
from bson.json_util import dumps, CANONICAL_JSON_OPTIONS

custom_Trend = 'trend'
client = pymongo.MongoClient("mongodb://lmcmz:a5654013@ds247759.mlab.com:47759/mydb")
db = client.mydb

def removeEmpty(tempList):
	tempList[:] = [item for item in tempList if item != '']
	return tempList

def createData(xlsxFile):
	suburb = os.path.basename(xlsxFile)
	suburb = suburb.replace(".xlsx","")
	suburb = suburb.lower()
	data = {"_id":suburb,suburb:{}}
	
	xwb = xlrd.open_workbook(xlsxFile)
	table = xwb.sheets()[0]
	
	group = table.col_values(0)
	group = group[7:69]
	group = removeEmpty(group)
	group = [x.lower() for x in group]

	type_key = table.col_values(1)
	type_key = type_key[7:69]

	for rownum in range(table.nrows):
		if rownum == 5:
			tempList = table.row_values(rownum)
			tempList = removeEmpty(tempList)
			new_list = []
			for item in tempList:
				temp = re.sub(r'\D+', "", item)
				new_list.append(temp)
			new_list.append(custom_Trend)

	for info in new_list:
		data[suburb][info] = {}
		for i,item in enumerate(group):
			data[suburb][info][item.lower()] = {}
			
	for info in new_list:        
		for i,item in enumerate(group):
			for j,t in enumerate(type_key):
				t = t.lower()
				if t == '':
					continue
				if j in range(17):
					data[suburb][info][group[0]][t] = {}
				if j in range(17,20):
					data[suburb][info][group[1]][t] = {}
				if j in range(20,21):
					data[suburb][info][group[2]][t] = {}
				if j in range(25,28):
					data[suburb][info][group[7]][t] = {}
				if j in range(29,45):
					data[suburb][info][group[9]][t] = {}
				if j in range(46,50):
					data[suburb][info][group[11]][t] = {}
				if j in range(54,60):
					data[suburb][info][group[16]][t] = {}

	dataList = []

	for colnum in range(2,15):
		tempList = table.col_values(colnum)
		new_tempList = []
		for i,item in enumerate(tempList):
			if i < 7  or i >68:
				continue
			if item == 'nc**' or "nc":
				new_tempList.append(item)
			if item == 'Stable':
				new_tempList.append(item)
			if isinstance(item, int):
				new_tempList.append(item)
			if isinstance(item, float):
				new_tempList.append(item)
			if colnum == 14 and i > 6 and i < 69:
				new_tempList.append(item)
		dataList.append(new_tempList)
	
#	for item in dataList:
#		print(len(item), item)
	
	descList = ['number of incidients', 'rate per 100,000 population']
	trendList = ['24 month trend','60 month trend','2016 LGA Rank']

	for i,subDict in enumerate(data[suburb]):
		count = 0
		for j,key in enumerate(data[suburb][subDict]):
			if subDict == custom_Trend:
				if len(data[suburb][subDict][key].keys()) > 0:
					for subKey in data[suburb][subDict][key]:
						data[suburb][subDict][key][subKey][trendList[0]] = dataList[2*i][count]
						data[suburb][subDict][key][subKey][trendList[1]] = dataList[2*i+1][count]
						data[suburb][subDict][key][subKey][trendList[2]] = dataList[2*i+2][count]
						count += 1
				else:
					data[suburb][subDict][key][trendList[0]] = dataList[2*i][count]
					data[suburb][subDict][key][trendList[1]] = dataList[2*i+1][count]
					data[suburb][subDict][key][trendList[2]] = dataList[2*i+2][count]
					count += 1
				continue
					
			if len(data[suburb][subDict][key].keys()) > 0:
				for subKey in data[suburb][subDict][key]:
					
					data[suburb][subDict][key][subKey][descList[0]] = dataList[2*i][count]
					data[suburb][subDict][key][subKey][descList[1]] = dataList[2*i+1][count]
					count += 1
			else:
				data[suburb][subDict][key][descList[0]] = dataList[2*i][count]
				data[suburb][subDict][key][descList[1]] = dataList[2*i+1][count]
				count += 1
	
	return data

def uploadToDB(jsonData, collection):
	if not collection in db.collection_names():
		print("Create collection on DataBase: "+ collection)
		db.create_collection(collection)
	crime = db[collection]
	crime.insert_one(jsonData)

def writeToFile(filename, jsonData):
	dir = "./data/lga/data/"
	if not os.path.exists(dir):
		os.makedirs(dir)
	filename = filename.replace(".xlsx","")
	with open(dir +filename, 'w') as f:
		f.write(jsonData)
		
def storeXlSXToDB(path, collection):
	data = createData(path)
	jsonData = json.dumps(data, indent=4)
	filename = (os.path.basename(path)).replace(".xlsx", ".json")
	writeToFile(filename, jsonData)
	uploadToDB(data, collection)
	print("Upload xlsx to DB")

def checkListExistInDB(suburbList, collection):
	fetch_list = []
	exist_list = []
	for sub in suburbList:
		if not checkExistInDB(sub, collection):
			fetch_list.append(sub)
		else:
			exist_list.append(sub)
	return fetch_list, exist_list

def checkExistInDB(suburb, collection):
	if not collection in db.collection_names():
		return None
	crime = db[collection]
	result = crime.find({suburb:{'$exists': True}})
	if result.count() == 0:
		return None
	return result

def getDataByFilter(filter, filterList, collection):
	if not collection in db.collection_names():
		return None
	crime = db[collection]
	result = crime.find(filter)
	if result.count() == 0:
		return None
	if len(filterList) > 1:
		return result[0][filterList[0]][filterList[1]]
	return result

def getListFile(collection, suburbList):
	if not collection in db.collection_names():
		return None
	all_dict = []
	crime = db[collection]
	
	for sub in suburbList:
		result = crime.find({sub:{'$exists': True}})
		if result.count() == 0:
			continue
		all_dict.append(result[0])
	return all_dict

def getFile(collection, suburb):
	if not collection in db.collection_names():
		return None
	crime = db[collection]
	result = crime.find({suburb:{'$exists': True}})
	if result.count() == 0:
		return None
	return result[0]

def getAllFile(collection):
	if not collection in db.collection_names():
		return None
	crime = db[collection]
	result = crime.find()
	if result.count() == 0:
		return None
	list = []
	for r in result:
		list.append(r)
	return list
	
	
def deleteFile(collection, suburb):
	if not collection in db.collection_names():
		return False,None
	crime = db[collection]
	result = crime.remove({suburb:{'$exists': True}})
	if result['n'] == 0:
		return False,result
	return True,result
		
		
