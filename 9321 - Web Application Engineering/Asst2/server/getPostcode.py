import requests,os,re
import xlstojson, json

baseURL = "http://www.bocsar.nsw.gov.au/Documents/RCS-Annual/"
dir = "./data/postcode"
dir_lga = "./data/lga/"

def is_downloadable(url):
	h = requests.head(url, allow_redirects=True)
	if not h.status_code == 200:
		return False
	header = h.headers
	content_type = header.get('content-type')
	if 'text' in content_type.lower():
		return False
	if 'html' in content_type.lower():
		return False
	return True

def is_downloadable_suburb(suburb):
	url = baseURL + str(suburb) + "lga.xlsx"
	h = requests.head(url, allow_redirects=True)
	if not h.status_code == 200:
		return False
	return True

def download_file(url, path):
	if not is_downloadable(url):
		return False
	r = requests.Response()
	if not os.path.isfile(path):
		r = requests.get(url)
		if r.status_code == 200:
			with open(path, 'wb') as f:
				f.write(r.content)
	return True
				
def getListLGAData(subList):
	for sub in subList:
		if not os.path.exists(dir_lga):
			os.makedirs(dir_lga)
		url = baseURL + sub + "lga.xlsx"
		path = dir_lga + sub + ".xlsx"
		download_file(url, path)

def getLGAData(sub):
	if not os.path.exists(dir_lga):
		os.makedirs(dir_lga)
	url = baseURL + sub + "lga.xlsx"
	path = dir_lga + sub + ".xlsx"
	download_file(url, path)
	return path

def getAllsuburb(cells, targetName):
	sub_list = []
	po_dict = {}
	for i,s in enumerate(cells):
		if s.value == targetName:
			postcode = sheet['C'][i].value
			postcode = "{0:.0f}".format(postcode)
			suburb = sheet['B'][i].value
			po_dict[postcode] = suburb
			if suburb not in sub_list:
				sub_list.append(suburb)
				
	new_list = []
	for sub in sub_list:
		new_str = sub.lower()
		new_str = re.sub(r"\s+", "", new_str)
		new_list.append(new_str)
	return new_list

def getTargetList(dict, target):
	new_dict = {}
	
	for key in dict:
		for sub in dict[key]:
			new_str = sub['lga_region'].lower()
			new_str = re.sub(r"\s+", "", new_str)
			postcode = "{0:.0f}".format(sub['postcode'])
			if not postcode in new_dict:
				new_dict[postcode] = []
			new_dict[postcode].append(new_str)
	return new_dict
	
def findSuburb(target):
	if not target.isdigit():
		target = target.lower()
		target = re.sub(r"\s+", "", target)
		print(target)
		if is_downloadable_suburb(target):
			return [target]
		else:
			return []		
	
	postcode_path = dir + "/postcode.xlsx"
	postcode_json_path = dir + "/postcode.json"
	postcode_txt_path = dir + "/postcode.txt"
	postcode_url = "https://docs.google.com/spreadsheets/d/1tHCxouhyM4edDvF60VG7nzs5QxID3ADwr3DGJh71qFg/export?format=xlsx&id=1tHCxouhyM4edDvF60VG7nzs5QxID3ADwr3DGJh71qFg"
	targetName = "New South Wales"
	suburbDict ={}
	
	if not os.path.exists(postcode_txt_path):
		os.makedirs(dir)
		success = download_file(postcode_url, postcode_path)
		jsonData, jsonPath = xlstojson.convertJson(postcode_path)
		dictData = json.loads(jsonData)
		suburbDict = getTargetList(dictData, targetName)
		suburbJson = json.dumps(suburbDict, indent=4)
		with open(postcode_txt_path, 'w') as f:
			f.write(suburbJson)
	else:
		with open(postcode_txt_path) as f:
			suburbDict = json.load(f)
		
	if target in suburbDict:
		return suburbDict[target]
	return []



