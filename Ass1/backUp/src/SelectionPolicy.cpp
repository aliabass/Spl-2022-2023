#include "../include/SelectionPolicy.h"

void MandatesSelectionPolicy::select(Simulation &s, int PartyId) {
    Graph a = s.getTheGraph();
    int maxMandates = -1;
    int MaxMandatesId = -1;
    bool sameCoalition = false;
    for (unsigned i = 0; i < (unsigned)a.getNumVertices(); i++) {//
        if (i != (unsigned)PartyId) {//
            if (a.getEdgeWeight(PartyId, i) > 0) {
                if (a.getTheParty(i).getState() == State::Waiting) {
                    if (a.getMandates(i) > maxMandates) {
                        maxMandates = a.getMandates(i);
                        MaxMandatesId = i;
                    }
                }
                if (a.getTheParty(i).getState() == State::CollectingOffers) {
                    if (a.getMandates(i) > maxMandates) {
                        sameCoalition = s.SameCoalitionMadeAnOffer(PartyId, i);
                        if (!sameCoalition) {
                            MaxMandatesId = i;
                            maxMandates = a.getMandates(i);
                        }
                    }
                }
            }
        }
        sameCoalition = false;
    }
    if (MaxMandatesId != -1) {
        s.makeOffer(MaxMandatesId, PartyId);
    }
}

string MandatesSelectionPolicy::getType() {
    return "M";
}

void EdgeWeightSelectionPolicy::select(Simulation &s, int PartyId) {
    Graph a = s.getTheGraph();
    int maxWeigh = -1;
    int MaxWeighId = -1;
    bool sameCoalition = false;
    for (unsigned i = 0; i < (unsigned)a.getNumVertices(); ++i) {
        if (i !=(unsigned)PartyId) {
            if (a.getEdgeWeight(PartyId, i) > 0) {
                if (a.getTheParty(i).getState() == State::Waiting) {
                    if (a.getEdgeWeight(PartyId, i) > maxWeigh) {
                        maxWeigh = a.getEdgeWeight(PartyId, i);
                        MaxWeighId = i;
                    }
                }
                if (a.getTheParty(i).getState() == State::CollectingOffers) {
                    if (a.getEdgeWeight(PartyId, i) > maxWeigh) {
                        sameCoalition = s.SameCoalitionMadeAnOffer(PartyId, i);
                        if (!sameCoalition) {
                            maxWeigh = a.getEdgeWeight(PartyId, i);
                            MaxWeighId = i;
                        }
                    }
                }
            }
        }
        sameCoalition = false;
    }
    if (MaxWeighId != -1) {
        s.makeOffer(MaxWeighId, PartyId);
    }
}

string EdgeWeightSelectionPolicy::getType() {
    return "E";
}


SelectionPolicy::~SelectionPolicy(){}
EdgeWeightSelectionPolicy::~EdgeWeightSelectionPolicy(){}
MandatesSelectionPolicy::~MandatesSelectionPolicy(){}
