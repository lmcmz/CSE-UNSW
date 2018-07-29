#!/usr/bin/python3

import spacy
nlp = spacy.load('en')
document = str(open("more").read())
document = nlp(document)

#一些参数定义
noisy_pos_tags = ["PROP"]
min_token_length = 2

#检查 token 是不是噪音的函数
def isNoise(token):     
	is_noise = False
	if token.pos_ in noisy_pos_tags:
		is_noise = True
	elif token.is_stop == True:
		is_noise = True
	elif len(token.string) <= min_token_length:
		is_noise = True
	return is_noise

def cleanup(token, lower = True):
	if lower:
	   token = token.lower()
	return token.strip()

# 评论中最常用的单词
from collections import Counter
cleaned_list = [cleanup(word.string) for word in document if not isNoise(word)]

# 检查修饰某个单词的所有形容词
def pos_words (sentence, token, ptag):
	sentences = [sent for sent in sentence.sents if token in sent.string]     
	pwrds = []
	for sent in sentences:
		for word in sent:
			if character in word.string:
				 pwrds.extend([child.string.strip() for child in word.children
				if child.pos_ == ptag] )
		return Counter(pwrds).most_common(10)

print(cleaned_list)
print(pos_words(document, 'hotel', "ADJ"))