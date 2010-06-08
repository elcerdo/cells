import actions

prout = 0

def act(player_name,agent_position,agent_arguments,agent_energy,agent_loaded,world_width,world_height):
    global prout
    print "player_name=",player_name
    print "agent_position =",agent_position
    print "agent_arguments =",agent_arguments
    print "agent_energy =",agent_energy,"agent_loaded =",agent_loaded
    print "world_size =",(world_width,world_height)
    prout += 1
    print "prout =",prout
    ret = actions.doNothing()
    print ret
    return ret

