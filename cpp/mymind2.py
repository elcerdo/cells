import actions
import random

def neighbor(p0,p1):
    dx = p0[0]-p1[0]
    dy = p0[1]-p1[1]
    return abs(dx)+abs(dy)

def act(player_name,agent_position,agent_arguments,agent_energy,agent_loaded,agents_viewed,world_width,world_height):
    ennemies = [agent[1] for agent in agents_viewed if agent[0]!=player_name and neighbor(agent_position,agent[1])<2]
    if ennemies:
        return actions.attack(*ennemies[0])

    if (agent_arguments):
       return actions.moveTo(agent_arguments[0],agent_arguments[1])
    else:
        if agent_energy>70:
            destx = random.randint(0,world_width-1)
            desty = random.randint(0,world_height-1)
            return actions.spawn(destx,desty,(destx,desty))
        else:
            return actions.eat()


