__all__ = ["Action","ACT_SPAWN","ACT_MOVE","ACT_EAT","ACT_ATTACK","ACT_LIFT","ACT_DROP"]

import actions

ACT_SPAWN, ACT_MOVE, ACT_EAT, ACT_ATTACK, ACT_LIFT, ACT_DROP = range(6)

print "imported dummy cells"

def Action(type,pos=None):
    if type==ACT_EAT: return actions.eat()
    elif type==ACT_LIFT: return actions.lift()
    elif type==ACT_DROP: return actions.drop()
    elif type==ACT_SPAWN: return actions.spawn(pos[0],pos[1],pos)
    elif type==ACT_MOVE: return actions.moveTo(*pos)
    elif type==ACT_ATTACK: return actions.attack(*pos)
    else: assert(False)

