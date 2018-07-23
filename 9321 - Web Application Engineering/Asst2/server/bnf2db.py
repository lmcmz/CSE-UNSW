import re

#lgaName   eq Bayside or lgaName eq   Ballina or lgaName eq Albury#
#{"$or": [{"albury":{'$exists': True}}, 
#		 {"bluemountains":{'$exists': True}}]}

#=============================================

#lgaName eq Bayside and year eq    2014
#{"albury.2013":{'$exists': True}}

def covert2DB(string):
	string = string.strip()
	list = re.split('or|and', string)
	clist = re.findall('or|and', string)
	dict = {}
	isOrType = 'or' in clist

	if isOrType:
		queryList = []
		for l in list:
			subDict = {}
			l = l.lower()
			l = l.strip()
			l = re.sub(r'\s+', " ", l)
			l = re.sub(r"lganame\ eq\ ", "", l)
			subDict[l] = {}
			subDict[l]['$exists'] = True
			queryList.append(subDict)
			dict['$or']	= queryList
		return dict, []

	lgaName = ""
	year = ""

	for l in list:
		l = l.lower()
		l = l.strip()
		l = re.sub(r'\s+', " ", l)
		if 'lganame' in l:
			lgaName = l.replace("lganame eq ", "")
		else:
			year = l.replace("year eq ", "")
		
	string = lgaName + '.' +year
	dict[string] = {}
	dict[string]['$exists'] = True
	return dict, [lgaName, year]
	


	
	
	