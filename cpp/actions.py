__all__=["doNothing","spawn","moveTo","eat"]

def doNothing():
    return [0]

def spawn(x,y,args):
    return [1,x,y]+list(args)

def moveTo(x,y):
    return [2,x,y]

def eat():
    return [3]

