#include <iostream>
#include "Simulation.h"

Simulation::Simulation(Graph graph, vector<Agent> agents) : mGraph(graph), mAgents(agents), Coalitions() {
    // You can change the implementation of the constructor, but not the signature!
    for (unsigned i = 0; i < (unsigned)agents.size(); i++) {//
        int x = agents[i].getPartyId();
        mGraph.getTheParty(x).setCoalition(i);
        mGraph.getTheParty(x).setCoolDown(-1);
        Coalition temp(i);
        Coalitions.push_back(temp);
        Coalitions[i].addToCoalition(x);
    }
}

void Simulation::step() {
    for (int i = 0; i < mGraph.getNumVertices(); ++i) {
        mGraph.getTheParty(i).step(*this);
    }
    for (unsigned i = 0; i < mAgents.size(); i++) {
        mAgents[i].step(*this);
    }
}

bool Simulation::shouldTerminate() const {
    vector<vector<int>> coalition = getPartiesByCoalitions();
    int sum;
    int sumOfJoined = 0;
    for (auto &i: coalition) {
        sum = 0;
        for (int j: i) {
            sum += mGraph.getParty(j).getMandates();
        }
        if (sum >= 61)
            return true;
    }
    for (auto &i: coalition) {
        for (int j: i) {
            if (mGraph.getParty(j).getState() == State::Joined) {
                sumOfJoined++;
            }
        }
    }
    if (sumOfJoined == mGraph.getNumVertices())
        return true;
    return false;
}

const Graph &Simulation::getGraph() const {
    return mGraph;
}

Graph &Simulation::getTheGraph() {
    return mGraph;
}

const vector<Agent> &Simulation::getAgents() const {
    return mAgents;
}

const Party &Simulation::getParty(int partyId) const {
    return mGraph.getParty(partyId);
}

Party &Simulation::getTheParty(int PartyId) {
    return mGraph.getTheParty(PartyId);
}

bool Simulation::SameCoalitionMadeAnOffer(int id1, int id2) {
    return mGraph.SameCoalitionMadeAnOffer(id1, id2);
}

int Simulation::numOfMandatesInCoalition(int PartyId) {
    int sum = 0;
    int coalitionNum = getTheParty(PartyId).getCoalition();
    vector<vector<int>> a = getPartiesByCoalitions();
    for (int i: a[coalitionNum]) {
        sum += getTheParty(i).getMandates();
    }
    return sum;
}

void Simulation::addAgentToVector(Agent &toAdd) {
    mAgents.push_back(toAdd);
}

void Simulation::makeOffer(int id, int offerFrom) {
    mGraph.makeOffer(id, offerFrom);
}


void Simulation::updateTheCloneAgent(int myId, int idToClone) {
    for (unsigned i = 0; i < (unsigned)mAgents.size(); i++) {
        if (mAgents[i].getPartyId() == idToClone) {
            Agent toAdd = mAgents[i];
            toAdd.setPartyId(myId);
            toAdd.setAgentId(mAgents.size());
            mAgents.push_back(toAdd);
        }
    }
}

void Simulation::addToCoalition(int PartyId, int Coalition) {
    Coalitions[Coalition].addToCoalition(PartyId);
}


/// This method returns a "coalition" vector, where each element is a vector of party IDs in the coalition.
/// At the simulation initialization - the result will be [[agent0.partyId], [agent1.partyId], ...]
const vector<vector<int>> Simulation::getPartiesByCoalitions() const {
    vector<vector<int>> ans;
    for (const auto & Coalition : Coalitions) {
        ans.push_back(Coalition.getMembers());
    }
    return ans;
}

