__all__ = ["Action","ACT_SPAWN","ACT_MOVE","ACT_EAT","ACT_ATTACK","ACT_LIFT","ACT_DROP"]

print "imported dummy cells"

import ccells

ACT_SPAWN, ACT_MOVE, ACT_EAT, ACT_ATTACK, ACT_LIFT, ACT_DROP = range(6)

def Action(type,pos=None):
    if type==ACT_EAT: return ccells.eat()
    elif type==ACT_LIFT: return ccells.lift()
    elif type==ACT_DROP: return ccells.drop()
    elif type==ACT_SPAWN: return ccells.spawn(pos[0],pos[1],list(pos))
    elif type==ACT_MOVE: return ccells.moveTo(*pos)
    elif type==ACT_ATTACK: return ccells.attack(*pos)
    else: assert(False)

