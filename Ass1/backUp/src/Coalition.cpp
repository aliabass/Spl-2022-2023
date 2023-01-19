#include "../include/Coalition.h"
#include <vector>
using std::vector;

Coalition::Coalition(int _CoalitionNum) : CoalitionNum(_CoalitionNum), membersOfCoalition(vector<int>()){}

void Coalition::addToCoalition(int id){
    membersOfCoalition.push_back(id);
}

const std::vector<int> & Coalition::getMembers() const {
    return membersOfCoalition;
}