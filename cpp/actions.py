__all__=["doNothing","spawn","moveTo","eat","attack","lift","drop"]

def doNothing():
    return [0]

def spawn(x,y,args):
    return [1,x,y]+list(args)

def moveTo(x,y):
    return [2,x,y]

def eat():
    return [3]

def attack(x,y):
    return [4,x,y]

def lift():
    return [5]

def drop():
    return [6]
