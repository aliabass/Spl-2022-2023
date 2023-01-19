#pragma once
#include "Simulation.h"

class SelectionPolicy {
public:
    virtual void select(Simulation & s,int id)=0;
    virtual ~SelectionPolicy();
    virtual string getType()=0;
};

class MandatesSelectionPolicy: public SelectionPolicy{
    void select(Simulation & s,int id) override;
    string  getType() override;
    ~MandatesSelectionPolicy();
};

class EdgeWeightSelectionPolicy: public SelectionPolicy{
    void select(Simulation & s,int id) override;
    string  getType() override;
    ~EdgeWeightSelectionPolicy();
};
