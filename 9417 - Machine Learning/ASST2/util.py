import json
import os

cwd = os.getcwd()

def save_file(item, filename, dirPath=cwd):
    if not os.path.exists(dirPath):
        os.makedirs(dirPath)
    with open(os.path.join(dirPath, filename), 'w') as f:
        json.dump(item, f, indent=4)
    f.close()


def load_file(filename, dirPath=cwd):
    with open(os.path.join(dirPath, filename), 'r') as f:
        data = json.load(f)
    f.close
    return data
