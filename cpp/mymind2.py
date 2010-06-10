import ccells
import random

def neighbor(p0,p1):
    dx = p0[0]-p1[0]
    dy = p0[1]-p1[1]
    return abs(dx)+abs(dy)

def act(player_name,agent_position,agent_arguments,agent_energy,agent_loaded,agents_viewed,plants_viewed,energy_map,world_width,world_height):
    ennemies = [agent[1] for agent in agents_viewed if agent[0]!=player_name and neighbor(agent_position,agent[1])<2]
    if ennemies:
        return ccells.attack(*ennemies[0])

    if agent_arguments and agent_arguments[0]==1:
        plants = [plant for plant in plants_viewed if neighbor(agent_position,plant[0])<2]
        if plants:
            plant_position = plants[0][0]
            plant_eff      = plants[0][1]

            occupied = False
            for agent_viewed in agents_viewed:
                if agent_viewed[1]==plant_position:
                    occupied = True
            if occupied:
                return ccells.moveTo(agent_arguments[1],agent_arguments[2])

            #print "found plant"
            #print plant_position,agent_position
            #print occupied
            if agent_energy>55:
                return ccells.spawn(plant_position[0],plant_position[1])
            else:
                return ccells.eat()
        return ccells.moveTo(agent_arguments[1],agent_arguments[2])
    else:
        if agent_energy>70:
            destx = random.randint(0,world_width-1)
            desty = random.randint(0,world_height-1)
            return ccells.spawn(destx,desty,[1,destx,desty,agent_position[0],agent_position[1]])
        else:
            return ccells.eat()


