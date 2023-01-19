#pragma once
#include <string>
#include "vector"
using std::string;

class JoinPolicy;
class Simulation;

enum State
{
    Waiting,
    CollectingOffers,
    Joined
};

class Party
{
public:
    Party(int id, string name, int mandates, JoinPolicy *);

    virtual ~Party();

    Party (const Party&other);

    Party& operator=(const Party& other);

    State getState() const;
    void setState(State state);
    int getMandates() const;
    void step(Simulation &s);
    const string &getName() const;
    std::vector<int>& getOffers();
    void setCoalition(int coalitionNum);
    int getCoalition() const;
    int getCoolDown() const;
    void setCoolDown(int newCoolDown);

private:
    int mId;
    string mName;
    int mMandates;
    JoinPolicy *mJoinPolicy;
    State mState;
    int coalition;
    int coolDown;
    std::vector<int> offers;
};
