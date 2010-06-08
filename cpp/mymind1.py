import actions

prout = 0

def act(player_name,agent_position,agent_arguments,agent_energy,agent_loaded,agents_viewed,world_width,world_height):
    global prout
    print "player_name=",player_name
    print "agent_position =",agent_position
    print "agent_arguments =",agent_arguments
    print "agent_energy =",agent_energy,"agent_loaded =",agent_loaded
    print "agent_viewed =",agents_viewed
    print "world_size =",(world_width,world_height)
    prout += 1
    print "prout =",prout
    ret = actions.doNothing()
    print ret
    return ret

