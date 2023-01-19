#include <iostream>
#include "Graph.h"

Graph::Graph(vector<Party> vertices, vector<vector<int>> edges) : mVertices(vertices), mEdges(edges) {
    // You can change the implementation of the constructor, but not the signature!
}

int Graph::getMandates(int partyId) const {
    return mVertices[partyId].getMandates();
}

int Graph::getEdgeWeight(int v1, int v2) const {
    return mEdges[v1][v2];
}

int Graph::getNumVertices() const {
    return mVertices.size();
}

const Party &Graph::getParty(int partyId) const {
    return mVertices[partyId];
}

Party &Graph::getTheParty(int PartyId) {
    return mVertices[PartyId];
}

vector<int> Graph::getOffers(int id) {
    return mVertices[id].getOffers();
}

bool Graph::SameCoalitionMadeAnOffer(int id1, int id2) {
    for (int offer : mVertices[id2].getOffers()) {
        if (getParty(id1).getCoalition() == getParty(offer).getCoalition())
            return true;
    }
    return false;
}

void Graph::makeOffer(int id, int offerFrom) {
    if (mVertices[id].getState() == State::CollectingOffers) {
        mVertices[id].getOffers().push_back(offerFrom);
    }
    if (mVertices[id].getState() == State::Waiting) {
        mVertices[id].setState(State::CollectingOffers);
        mVertices[id].getOffers().push_back(offerFrom);
    }
}
