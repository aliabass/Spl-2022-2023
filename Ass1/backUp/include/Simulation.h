#pragma once

#include <vector>
#include "Coalition.h"
#include "Graph.h"
#include "Agent.h"

using std::string;
using std::vector;

class Simulation {
public:
    Simulation(Graph g, vector<Agent> agents);

    void step();

    bool shouldTerminate() const;

    const Graph &getGraph() const;

    [[maybe_unused]] Graph &getTheGraph(); //to avoid an unused warning

    const vector<Agent> &getAgents() const;

    const Party &getParty(int partyId) const;

    [[maybe_unused]] bool SameCoalitionMadeAnOffer(int id1, int id2);

    const vector<vector<int>> getPartiesByCoalitions() const;

    [[maybe_unused]] int numOfMandatesInCoalition(int PartyId);

    Party &getTheParty(int PartyId);

    [[maybe_unused]] void addAgentToVector(Agent &);

    [[maybe_unused]] void makeOffer(int id, int offerFrom);

    [[maybe_unused]] void updateTheCloneAgent(int myId, int idToClone);

    [[maybe_unused]] void addToCoalition(int PartyId,int Coalition);



private:
    Graph mGraph;
    vector<Agent> mAgents;
    vector<Coalition> Coalitions;
};
