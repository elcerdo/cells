import ccells

prout = 0

def act(player_name,agent_position,agent_arguments,agent_energy,agent_loaded,agents_viewed,plants_viewed,energy_map,world_width,world_height):
    global prout
    print "player_name=",player_name
    print "agent_position =",agent_position
    print "agent_arguments =",agent_arguments
    print "agent_energy =",agent_energy,"agent_loaded =",agent_loaded
    print "agents_viewed =",agents_viewed
    print "plants_viewed =",plants_viewed
    print "world_size =",(world_width,world_height)
    print "energy_map =",len(energy_map)
    print ccells.__doc__
    prout += 1
    print "prout =",prout
    print "messages =",ccells.getMessages()
    ret = ccells.doNothing()
    ret = ccells.spawn(10,10,[23,42])
    ret = ccells.moveTo(10,14)
    ret = ccells.eat()
    ret = ccells.attack(10,23)
    ret = ccells.lift()
    ret = ccells.drop()
    print ret
    ccells.sendMessage("hello from prout = %d" % prout)
    return ret

