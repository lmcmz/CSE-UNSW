from math import sqrt

# the person correlation coefficient of user1 and user2
def sim_person(prefs, user1, user2):
    # build the dict of co-rated items of user1 and user2
    si = {}
    for item in prefs[user1]:
        if item in prefs[user2]:
            si[item] = 1

    # if there are no ratings in common, return 0
    if len(si) == 0: return 0

    # the number of the corrated items
    n = len(si)
    
    # sum of all ratings
    sum1 = float(sum([prefs[user1][it] for it in si]))
    sum2 = float(sum([prefs[user2][it] for it in si]))

    # Sums of the squared ratings
    sum1Sq = float(sum([pow(prefs[user1][it], 2) for it in si]))
    sum2Sq = float(sum([pow(prefs[user2][it], 2) for it in si]))

    # Sum of the products between user1 and user2
    pSum = float(sum([prefs[user1][it] * prefs[user2][it] for it in si]))

    # Calculate r (person score)
    num = float(pSum - (sum1 * sum2 / n))
    den = float(sqrt((sum1Sq - pow(sum1, 2) / n) * (sum2Sq - pow(sum2, 2) / n)))
    if den == 0:
        return 0
    r = float(num / den)
    return round(r, 7)


def sim_person_improved(prefs, user1, user2):
    # build the dict of co-rated items of user1 and user2
    si = {}
    for item in prefs[user1]:
        if item in prefs[user2]:
            si[item] = 1
    #if there is no items in common.
    if len(si) == 0:
        return 0

    #number of corated items
    n = len(si)

    #the number of items which have been rated by users
    count1 = len(prefs[user1])
    count2 = len(prefs[user2])

    totalCount = count1 + count2 - n

    #sum up all ratings of a single person
    sum1 = sum([prefs[user1][item] for item in si])
    sum2 = sum([prefs[user2][item] for item in si])

    #the mean of each person
    mean1 = sum1 / n;
    mean2 = sum2 / n;

    #covaraince of each pair
    covariance = sum([(prefs[user1][item] - mean1) * (prefs[user2][item] - mean2) for item in si]) / n
    #standard devariation
    sd1 = sqrt(sum([pow(prefs[user1][item] - mean1, 2) for item in si]) / n)
    sd2 = sqrt(sum([pow(prefs[user2][item] - mean2, 2) for item in si]) / n)
    if sd1 * sd2 == 0:
        return 0
    person = (covariance / (sd1 * sd2)) * (float(n) / float(totalCount))
    return person

def cal_distance(likes, movie1, movie2):
    count = {};
    for movie in likes[movie1]:
        if movie in likes[movie2]:
            count[movie] = 1;
    if len(count) == 0:
        return 0;
    sum_squares = sum(
        [pow(likes[movie1][movie] - likes[movie2][movie], 2) for movie in likes[movie1] if movie in likes[movie2]])
    return (1 / (1 + sqrt(sum_squares)))


def predict_cosine_improved(likes, movie1, movie2):
    count = {}
    for i in likes[movie1]:
        if i in likes[movie2]:
            count[i] = 1
    n = len(count)
    if n == 0: return 0
    count1 = 0
    count2 = 0
    for movie in likes[movie1]:
        count1 += 1
    for movie in likes[movie2]:
        count2 += 1
    totalCount = count1 + count2 - n
    x = sqrt(sum([likes[movie1][it] ** 2 for it in count]))
    y = sqrt(sum([likes[movie2][it] ** 2 for it in count]))
    xy = sum([likes[movie1][it] * likes[movie2][it] for it in count])
    cos = xy / (x * y)
    return cos * (float(n) / float(totalCount))


def predict_cosine_improved_tag(likes, movie1, movie2, movieTags):
    common = 0
    for i in movieTags[movie1]:
        if i in movieTags[movie2]:
            common += 1
    if common >= 5:
        return 0.8
    else:
        count = {}
        for i in likes[movie1]:
            if i in likes[movie2]:
                count[i] = 1
        #print count
        n = len(count)
        if n == 0:
            return 0
        count2 = 0
        count1 = 0
        for movie in likes[movie2]:
            count2 += 1
        for movie in likes[movie1]:
            count1 += 1
        totalCount = count1 + count2 - n
        x = sqrt(sum([likes[movie1][it] ** 2 for it in count]))
        y = sqrt(sum([likes[movie2][it] ** 2 for it in count]))
        xy = sum([likes[movie1][it] * likes[movie2][it] for it in count])
        cos = xy / (x * y)
        return cos * (float(n) / float(totalCount))
