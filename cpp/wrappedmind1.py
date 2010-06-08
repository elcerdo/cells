import actions

class WrappedMe:
    def __init__(self,position,team):
        self.position = position
        self.team = team

    def get_pos(self):
        return self.position

class WrapperView:
    def __init__(self,player_name,agent_position):
        self.me = WrappedMe(agent_position,player_name)

    def get_me(self):
        return self.me

    def get_agents(self):
        return None

def wrapper(wrapped_module_name,class_init_arg=None):
    wrapped_module = __import__(wrapped_module_name)
    instances = {}

    def act(player_name,agent_position,agent_arguments,agent_energy,agent_loaded,world_width,world_height):
        try:
            instance = instances[player_name]
        except KeyError: # first time we have to instanciate the class
            instance = wrapped_module.AgentMind(class_init_arg)
            instances[player_name] = instance
        finally:
            view = WrapperView(player_name,agent_position)
            action = instance.act(view,None)
            return actions.doNothing()


    return act
    
act = wrapper("mind1")
