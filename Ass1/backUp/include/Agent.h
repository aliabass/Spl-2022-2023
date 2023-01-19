#pragma once

#include <vector>
#include "Graph.h"

class SelectionPolicy;

class Agent {
public:
    Agent(int agentId, int partyId, SelectionPolicy *selectionPolicy);

    virtual ~Agent();

    Agent(const Agent &other);

    Agent& operator=(const Agent& other);


    int getPartyId() const;

    int getId() const;

    void step(Simulation &);

    void setAgentId(int AgentId);

    void setPartyId(int PartyId);


private:
    int mAgentId;
    int mPartyId;
    SelectionPolicy *mSelectionPolicy;
};
