import os
import util
from recommandSystem import RecommandSystem

save_file = 'sim_user.json'
result_file = 'new_result.txt'

class UserRecommandSystem(RecommandSystem):
    def __init__(self, method, train_path, val_path, top_k):
        RecommandSystem.__init__(self, method,train_path=train_path, 
                                val_path=val_path, top_k=top_k)
        self.simi_data = {}

    def sim_user(self):
        simi_users = {}
        for i in self.train_data.keys():
            simi_users.setdefault(i, [])

        for user in self.train_data:
            scores = self.calulate_similarity(self.train_data ,user)
             # using person as the key for its scores
            simi_users[user] = scores
        util.save_file(simi_users, save_file)
        self.simi_data = simi_users
    
    def load_sim_file(self):
        self.simi_data = util.load_file(save_file)

    # def predict_movie(self, user, movie):
    #     sum_ratings = 0.0
    #     similarity_sum = 0.0
    #     sim = 0.0
    #     predict = 0
    #     if self.simi_data == {}:
    #         self.sim_user()
    #     # loop over all user in the list
    #     for simi_user in self.simi_data[user]:
    #         if simi_user[1] == user:
    #             continue
    #         sim = simi_user[0]
    #         if sim <= 0: 
    #             continue
    #         # do predict for all those item which the user have not seen yet or have not rated yet
    #         if movie not in self.train_data[user] or self.train_data[user][movie] == 0:
    #             if movie in self.train_data[simi_user[1]]:
    #                 # get weighted ratings from all users in similar
    #                 sum_ratings += self.train_data[simi_user[1]][movie] * sim
    #                 similarity_sum += sim
    #     # case: if there is no score ,set this predicting as 4 score
    #     if similarity_sum == 0:
    #         predict = 4.0
    #     else:
    #         predict = sum_ratings / similarity_sum
    #     return predict
    
    def predict_result(self):
        with open(result_file, 'w') as f:
          for val in self.val_data:
            predict = self.predict_movie(val[0], val[1])
            # print(p[0], p[1], predict)
            f.write(val[0] + '\t' + val[1] + '\t' + str(predict) + '\r\r\n')
        f.close()
    
    def produce_rec_items(self, person):
        #prefs = self.produce_training_data()
        self.load_train_data()
        sum_rating_list = {}
        similarity_sum_dict = {}
        sim = 0.0
        # loop over all the similar persons in first n top match set.
        for simi_user in self.calulate_similarity( self.train_data,person):
            # filter those results which is rated by user's own
            if simi_user[1] == person: continue
            sim = simi_user[0]
            # do not care those similarity whcih is lower than 0

            if sim <= 0: continue
            for movie in self.train_data[simi_user[1]]:
                # only those item that the userhave not seen and has not been rated yet needed to be done.
                if movie not in self.train_data[person] or self.train_data[person][movie] == 0:
                
                    sum_rating_list.setdefault(movie, 0)
                    # get the raking by gathering all the similar users' rating multiply their sim
                    sum_rating_list[movie] += self.train_data[simi_user[1]][movie] * sim
                    #sum of similarities
                    similarity_sum_dict.setdefault(movie, 0)
                    similarity_sum_dict[movie] += sim

        #create the normalized list
        rankings = [(total / similarity_sum_dict[movie], movie) for movie, total in sum_rating_list.items()]

        #return the sorted list
        rankings.sort()
        rankings.reverse()
        return rankings

    

