import os
import glob, re

stop_words = set({'ourselves', 'hers', 'between', 'yourself', 'again', 'there', 'about', 'once', 'during', 'out', 'very', 'having', 'with', 'they', 'own', 'an', 'be', 'some', 'for', 'do', 'its', 'yours', 'such', 'into', 'of', 'most', 'itself', 'other', 'off', 'is', 's', 'am', 'or', 'who', 'as', 'from', 'him', 'each', 'the', 'themselves', 'below', 'are', 'we', 'these', 'your', 'his', 'through', 'don', 'me', 'were', 'her', 'more', 'himself', 'this', 'down', 'should', 'our', 'their', 'while', 'above', 'both', 'up', 'to', 'ours', 'had', 'she', 'all', 'no', 'when', 'at', 'any', 'before', 'them', 'same', 'and', 'been', 'have', 'in', 'will', 'on', 'does', 'yourselves', 'then', 'that', 'because', 'what', 'over', 'why', 'so', 'can', 'did', 'not', 'now', 'under', 'he', 'you', 'herself', 'has', 'just', 'where', 'too', 'only', 'myself', 'which', 'those', 'i', 'after', 'few', 'whom', 't', 'being', 'if', 'theirs', 'my', 'against', 'a', 'by', 'doing', 'it', 'how', 'further', 'was', 'here', 'than', 'but', 'br', 'little', 'him', 'her', 'many', 'however', 'one', 'still', 'two', 'much', 'call', 'beside', 'get', 'due', 'indeed', 'five', 'already', 'besides', 'either', 'least', 'keep', 'whence', 'quite', 'within', 'could', 'say', 'using', 'whereas', 're', 'hereupon', 'meanwhile', 'whereupon', 'wherein', 'third', 'hereafter', 'done', 'name', 'top', 'four', 'well', 'amongst', 'whenever', 'seems', 'forty', 'herein', 'must', 'mostly', 'fifteen', 'make', 'beforehand', 'take', 'anywhere', 'every', 'per', 'become', 'moreover', 'seem', 'full', 'empty', 'put', 'sixty', 'side', 'three', 'hereby', 'serious', 'please', 'first', 'anyway', 'else', 'twelve', 'afterwards', 'thereupon', 'also', 'became', 'several', 'almost', 'everyone', 'via', 'elsewhere', 'until', 'made', 'last', 'nowhere', 'along', 'whereby', 'sometimes', 'show', 'even', 'around', 'whither', 'formerly', 'various', 'move', 'without', 'cannot', 'give', 'ten', 'beyond', 'another', 'eight', 'always', 'upon', 'across', 'somewhere', 'onto', 'anyhow', 'fifty', 'thru', 'often', 'mine', 'seeming', 'toward', 'us', 'although', 'see', 'really', 'ever', 'bottom', 'latter', 'since', 'eleven', 'together', 'whose', 'none', 'front', 'seemed', 'someone', 'twenty', 'hundred', 'nobody', 'everywhere', 'everything', 'nevertheless', 'perhaps', 'would', 'next', 'thereafter', 'less', 'whatever', 'neither', 'nothing', 'go', 'becomes', 'latterly', 'towards', 'throughout', 'may', 'alone', 'others', 'nor', 'part', 'thence', 'though', 'unless', 'regarding', 'somehow', 'behind', 'back', 'whoever', 'therein', 'sometime', 'whereafter', 'amount', 'rather', 'thereby', 'therefore', 'used', 'might', 'whether', 'wherever', 'hence', 'anything', 'never', 'yet', 'six', 'anyone', 'whole', 'becoming', 'enough', 'except', 'former', 'noone', 'thus', 'nine', 'something', 'among', 'otherwise', 'namely', 'ca', 'saw', 'etc', 'let', 'wasn', 'oh', 'to', 'jo'})

def preprocess(review):
		"""
		Apply preprocessing to a single review. You can do anything here that is manipulation
		at a string level, e.g.
				- removing stop words
				- stripping/adding punctuation
				- changing case
				- word find/replace
		RETURN: the preprocessed review in string form.
		"""
		
		processed_review = []
		string = review.lower();
		tokens = [i for i in re.split(r'([\d.]+|\W+)', string) if i]
		for token in tokens:
			if re.search("\W+", token):
				continue  
			if re.search("\s+", token):
				continue  
			if re.search("[.,$-;:<>]", token):
				continue
			if token in stop_words:
				continue
			if len(token) < 3:
				continue
			processed_review.append(token)
		return processed_review


all_dict = dict()

print("Loading IMDB Data...")
#data = []
path='./data/train'
dir = os.path.dirname(__file__)
file_list = glob.glob(os.path.join(dir, path + '/pos/*'))
file_list.extend(glob.glob(os.path.join(dir, path + '/neg/*')))
print("Parsing %s files" % len(file_list))
for i, f in enumerate(file_list):
	with open(f, "r") as openf:
		s = openf.read()
		list_l = preprocess(s)
		for w in list_l:
			if w in all_dict:
				all_dict[w] += 1
			else:
				all_dict[w] = 1


sorted_by_value = sorted(all_dict.items(), key=lambda kv: kv[1])

print(sorted_by_value)

#for (k, v) in sorted_by_value:
#	if (v < 3):
#		print("'",end="")
#		print(k, end="")
#		print("',",end="")