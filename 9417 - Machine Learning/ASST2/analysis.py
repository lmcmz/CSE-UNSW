import os
from math import sqrt

class Analysis:
    def __init__(self, testFile,i, rec, pathStr=os.getcwd() + '/ml-100k', userFile='/u.user'):
        self.testFile = testFile
        self.rec = rec
        self.pathStr = pathStr
        self.userFile = userFile
        self.num = i
    def get_test_user(self):
        # build a empty dict for storing test users
        user_list = {}
        try:
            # open user files
            with open(self.pathStr + self.userFile) as user:
                for line in user:
                    (uid, _) = line.split('|')[0:2]
                    user_list.setdefault(uid, {})
        except IOError as err:
            print ('File error: ' + str(err))
        try:
            # open test files
            with open(self.pathStr + self.testFile) as t:
                for line in t:
                    (uid, iid, rating, _) = line.split('\t')
                    user_list[uid][iid] = float(rating)
        except IOError as err:
            print ('File error: ' + str(err))
        return user_list

    def produce_result(self):
        s_MAE = 0
        s_RMSE = 0
        number = 0
        # get all the test users
        testingSet = self.get_test_user()
        # print(len(testingSet))
        for user in testingSet:
            # get all recommended items for the user
            recList = self.rec.produce_rec_items(user)
            for recItem in recList:
                if recItem[1] in testingSet[user]:
                    number += 1
                    # get the diff between testing set and training set
                    dif = abs(recItem[0] - testingSet[user][recItem[1]])
                    # squared differences
                    s_RMSE += dif ** 2
                    # absolute differences
                    s_MAE += dif

        MAE = s_MAE / number
        RMSE = sqrt(s_RMSE / number)
        return MAE, RMSE, number
        # return 0,0,0
