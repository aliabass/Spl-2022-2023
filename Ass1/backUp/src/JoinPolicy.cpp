
#include "../include/JoinPolicy.h"


void MandatesJoinPolicy::join(Simulation &s, vector<int> &offers, int myPartyId) {
    int MaxMandates = -1;
    int MaxMandatesId = -1;
    for (unsigned i = 0; i < offers.size(); i++) {
        if (s.numOfMandatesInCoalition(offers[i]) > MaxMandates) {
            MaxMandates = s.numOfMandatesInCoalition(offers[i]);
            MaxMandatesId = offers[i];
        }
    }
    s.getTheParty(myPartyId).setState(State::Joined);
    s.getTheParty(myPartyId).setCoalition(s.getTheParty(MaxMandatesId).getCoalition());
    s.addToCoalition(myPartyId, s.getTheParty(MaxMandatesId).getCoalition());
    s.getTheParty(myPartyId).setCoolDown(-1);
    s.updateTheCloneAgent(myPartyId, MaxMandatesId);
}

string MandatesJoinPolicy::getType() {
    return "M";
}

void LastOfferJoinPolicy::join(Simulation &s, vector<int> &offers, int myPartyId) {
    s.getTheParty(myPartyId).setState(State::Joined);
    s.getTheParty(myPartyId).setCoalition(s.getTheParty(offers[offers.size() - 1]).getCoalition());
    s.addToCoalition(myPartyId, s.getTheParty(offers[offers.size() - 1]).getCoalition());
    s.getTheParty(myPartyId).setCoolDown(-1);
    s.updateTheCloneAgent(myPartyId, offers[offers.size() - 1]);
}

string LastOfferJoinPolicy::getType() {
    return "L";
}


LastOfferJoinPolicy::~LastOfferJoinPolicy(){}
MandatesJoinPolicy::~MandatesJoinPolicy(){}
JoinPolicy::~JoinPolicy(){}
