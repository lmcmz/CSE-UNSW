import os

class RecommandSystem:
    def __init__(self, method, train_path, val_path, top_k=30):
        self.method = method
        self.train_path = train_path
        self.val_path = val_path
        self.train_data = {}
        self.val_data= {}
        self.simi_data={}
        self.top_k = top_k
        self.load_train_data()
        self.load_val_data()
    
    def load_train_data(self):
        train_data = {}
        with open(self.train_path) as train:
            for line in train:
                (userId, movieId, rating, _) = line.split('\t')
                train_data.setdefault(userId, {})
                train_data[userId][movieId] = float(rating)
        self.train_data = train_data

    def load_val_data(self):
        val_data = []
        with open(self.val_path) as predict:
            for line in predict:
                (userId, movieId, _, _) = line.split('\t')
                movieId = movieId.replace('\r\r\n', '')
                val_data.append((userId, movieId))
        self.val_data = val_data
    
    def calulate_similarity(self, data,target):
        scores = []
        for other in data:
            if other != target:
                value = (self.method(data, target, other) ,other)
                scores.append(value)
        scores.sort()
        scores.reverse()
        return scores[0:self.top_k]

    def predict_movie(self, user, movie):
        totals = 0.0
        simSums = 0.0
        sim = 0.0
        predict = 0
        if movie not in self.simi_data:
            predict = 4.0
            return predict
        for other in self.simi_data[movie]:
            if other[1] == movie:
                continue
            sim = other[0]
            sim_movie = other[1]
            if sim <= 0:
                continue
            if movie not in self.train_data[user] or self.train_data[user][item] == 0:
                if sim_movie in self.train_data[user]:
                    totals += self.train_data[user][sim_movie] * sim
                    simSums += sim
        if simSums == 0:
            predict = 4.0
        else:
            predict = totals / simSums
        return predict

    


