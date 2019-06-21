import json
import os.path

my_path = os.path.abspath(os.path.dirname(__file__))
path = os.path.join(my_path, "data.json")

data = None
def readData():
    global data
    if data != None:
        return
    with open(path, 'r') as f:
        data = json.load(f)
        # print(data)
        f.close()

def writeData(jsonData):
    with open(path, 'w') as f:
        json.dump(jsonData, f, indent=4)
        f.close()