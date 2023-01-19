#pragma once
#include "vector"

class Coalition{
private:
    int CoalitionNum;
    std::vector<int> membersOfCoalition;

public:
    explicit Coalition(int CoalitionNum);
    void addToCoalition(int id);
    const std::vector<int> & getMembers() const;
};