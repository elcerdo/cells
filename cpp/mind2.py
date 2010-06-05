import actions
import random

def act(player_name,agent_position,agent_arguments,agent_energy,agent_loaded,world_width,world_height):
    if (agent_arguments):
       return actions.moveTo(agent_arguments[0],agent_arguments[1])
    else:
        if agent_energy>70:
            destx = random.randint(0,world_width-1)
            desty = random.randint(0,world_height-1)
            return actions.spawn(destx,desty,(destx,desty))
        else:
            return actions.eat()


