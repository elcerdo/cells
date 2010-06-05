__all__=["doNothing"]

def doNothing():
    return [0]

def spawn(x,y,args):
    return [1,x,y]+args

def move(x,y):
    return [2,x,y]

def eat():
    return [3]

