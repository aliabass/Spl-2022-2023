//adding move constructor?

#include "Agent.h"
#include "SelectionPolicy.h"

Agent::Agent(int agentId, int partyId, SelectionPolicy *selectionPolicy) : mAgentId(agentId), mPartyId(partyId),
                                                                           mSelectionPolicy(selectionPolicy) {
    // You can change the implementation of the constructor, but not the signature!
}

//destructor
Agent::~Agent() {
    if (mSelectionPolicy) {
        delete mSelectionPolicy;
    }
}

//copy constructor
Agent::Agent(const Agent &other) : mAgentId(other.mAgentId), mPartyId(other.mPartyId),
                                   mSelectionPolicy(other.mSelectionPolicy){
    mAgentId = other.getId();
    mPartyId = other.getPartyId();
    if (other.mSelectionPolicy->getType() == "M") {
        mSelectionPolicy = new MandatesSelectionPolicy;
    } else {
        mSelectionPolicy = new EdgeWeightSelectionPolicy;
    }
}

//assignment operator
Agent &Agent::operator=(const Agent &other) {
    if (this != &other) {
        if (mSelectionPolicy) {
            delete mSelectionPolicy;
        }
        mPartyId = other.getPartyId();
        mAgentId = other.getId();
        if (other.mSelectionPolicy->getType() == "M") {
            mSelectionPolicy = new MandatesSelectionPolicy;
        } else {
            mSelectionPolicy = new EdgeWeightSelectionPolicy;
        }
    }
    return *this;//
}

int Agent::getId() const {
    return mAgentId;
}

int Agent::getPartyId() const {
    return mPartyId;
}

void Agent::step(Simulation &sim) {
    mSelectionPolicy->select(sim, getPartyId());
}

void Agent::setAgentId(int AgentId) {
    mAgentId=AgentId;
}

void Agent::setPartyId(int PartyId) {
    mPartyId=PartyId;
}
