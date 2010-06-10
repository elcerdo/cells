#ifndef __MINDS_H__
#define __MINDS_H__

#include "interface.h"

// debug mind: print args, do nothing
struct MindTest1 : public Mind {
    virtual Action act(const AgentMe &me) const;
};

// spawer mind: spawn agent from plant when it as enough energy
struct MindTest2 : public Mind {
    virtual Action act(const AgentMe &me) const;
};

// same as mind2 but doesnt print anything
struct MindTest3 : public Mind {
    virtual Action act(const AgentMe &me) const;
};

#endif
