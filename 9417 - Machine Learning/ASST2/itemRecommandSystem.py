import os
import util
from recommandSystem import RecommandSystem

save_file = 'sim_item.json'
result_file = 'new_item_result.txt'

class ItemRecommandSystem(RecommandSystem):
    def __init__(self, method, train_path, val_path, top_k):
        RecommandSystem.__init__(self, method,train_path=train_path, 
                                val_path=val_path, top_k=top_k)
        self.simi_data = {}
    
    
    def switch_key(self):
        result = {}
        for person in self.train_data:
            for item in self.train_data[person]:
                result[item] = {}
                result[item][person] = self.train_data[person][item]
        return result

    def sim_item(self):
        simi_items = {}
        re_train_data  = self.switch_key()
        # print(self.train_data)
        for i in re_train_data.keys():
            simi_items[i] = []
        for user in re_train_data:
            scores = self.calulate_similarity(re_train_data ,user)
            simi_items[user] = scores
        util.save_file(simi_items, save_file)
        self.simi_data = simi_items
    
    def load_sim_file(self):
        self.simi_data = util.load_file(save_file)

    def predict_result(self):
        with open(result_file, 'w') as f:
          for val in self.val_data:
            predict = self.predict_movie(val[0], val[1])
            # print(p[0], p[1], predict)
            f.write(val[0] + '\t' + val[1] + '\t' + str(predict) + '\r\r\n')
        f.close()
    
    def produce_rec_items(self, user):
        self.load_train_data()
        self.load_sim_file()
        userRatings = self.train_data[user]
        scores = {}
        totalSim = {}
        for (item, rating) in userRatings.items():
            try:
                self.simi_data[item]
            except Exception:
                continue
            for (similarity, item2) in self.simi_data[item]:
                if similarity <= 0: continue
                if item2 in userRatings: continue
                scores.setdefault(item2, 0)
                scores[item2] += similarity * rating
                totalSim.setdefault(item2, 0)
                totalSim[item2] += similarity
        rankings = [(round(score / totalSim[item], 7), item) for item, score in scores.items()]
        rankings.sort()
        rankings.reverse()
        return rankings
