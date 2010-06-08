import actions

class WrappedMe:
    def __init__(self,team,position,args,energy,loaded,view):
        self.position = position
        self.team = team
        self.args = args
        self.energy = energy
        self.loaded = loaded
        self.view = view
    def get_pos(self):
        return self.position
    def get_team(self):
        return self.team
    def get_view(self):
        return self.view

class WrapperAgent:
    def __init__(self,team,position):
        self.team = team
        self.position = position
    def get_pos(self):
        return self.position
    def get_team(self):
        return self.team

class WrapperPlant:
    def __init__(self,position,eff):
        self.position = position
        self.eff = eff
    def get_pos(self):
        return self.position
    def get_eff(self):
        return self.eff

class WrapperMap:
    def __init__(self,data,width,height):
        self.data = data
        self.width = width
        self.height = height
    def get(self,x,y):
        return self.data[x*self.height+y]

class WrapperView:
    def __init__(self,player_name,agent_position,agent_arguments,agent_energy,agent_loaded,agents_viewed,plants_viewed,energy_map,world_width,world_height):
        self.me = WrappedMe(player_name,agent_position,agent_arguments,agent_energy,agent_loaded,self)
        self.agents = [WrapperAgent(agent[0],agent[1]) for agent in agents_viewed]
        self.plants = [WrapperPlant(plant[0],plant[1]) for plant in plants_viewed]
        self.energy = WrapperMap(energy_map,world_width,world_height)
    def get_me(self):
        return self.me
    def get_agents(self):
        return self.agents
    def get_plants(self):
        return self.plants
    def get_energy(self):
        return self.energy

def wrapper(wrapped_module_name,class_init_arg=None):
    wrapped_module = __import__(wrapped_module_name)
    instances = {}

    def act(player_name,agent_position,agent_arguments,agent_energy,agent_loaded,agents_viewed,plants_viewed,energy_map,world_width,world_height):
        try:
            instance = instances[player_name]
        except KeyError: # first time we have to instanciate the class
            instance = wrapped_module.AgentMind(class_init_arg)
            instances[player_name] = instance
        finally:
            view = WrapperView(player_name,agent_position,agent_arguments,agent_energy,agent_loaded,agents_viewed,plants_viewed,energy_map,world_width,world_height)
            action = instance.act(view,None)
            return action

    return act
    
act = wrapper("mind1")
