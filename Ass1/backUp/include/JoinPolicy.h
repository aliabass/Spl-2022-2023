#pragma once
//#ifndef JoinPolicy
//#define JoinPolicy

#include "Simulation.h"

class JoinPolicy {
public:
    virtual void join(Simulation &s, vector<int> &offers,int myPartyId) = 0;
    virtual string getType()=0;
    virtual ~JoinPolicy();
};

class MandatesJoinPolicy : public JoinPolicy {
    void join(Simulation &s, vector<int> &offers,int myPartyId) override;
    string getType() override;
    ~MandatesJoinPolicy();
};

class LastOfferJoinPolicy : public JoinPolicy {
    void join(Simulation &s, vector<int> &offers,int myPartyId) override;
    string getType() override;
    ~LastOfferJoinPolicy();
};
