import os
from recommandSystem import RecommandSystem
from userRecommandSystem import UserRecommandSystem
from itemRecommandSystem import ItemRecommandSystem
import method
from analysis import Analysis

def userBase(top_k=10):
    rec = UserRecommandSystem(method.sim_person,
                        train_path = os.path.join('userbase_new', 'dataset','ua.base'),
                        val_path = os.path.join('userbase_new', 'dataset','ua.test'),
                        top_k = top_k)
    rec.sim_user()
    rec.load_sim_file()
    rec.predict_result()

    # print(rec.simi_data)
    ana = Analysis('/ua.test',top_k, rec=rec)
    print(ana.produce_result())

def itemBase(top_k=10):
    rec = ItemRecommandSystem(method.predict_cosine_improved,
                            train_path = os.path.join('userbase_new', 'dataset','ua.base'),
                            val_path = os.path.join('userbase_new', 'dataset','ua.test'),
                            top_k = top_k)
    rec.sim_item()
    rec.load_sim_file()
    rec.predict_result()
    ana = Analysis('/ua.test',top_k, rec=rec)
    print(ana.produce_result())
    # rec.load_sim_file()
    # print(rec.simi_data)

if __name__ == '__main__':
    # userBase()
    itemBase()
