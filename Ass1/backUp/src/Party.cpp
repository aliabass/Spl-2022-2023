//adding move constructor?

#include "Party.h"
#include "JoinPolicy.h"

Party::Party(int id, string name, int mandates, JoinPolicy *jp) : mId(id), mName(name), mMandates(mandates),
                                                                  mJoinPolicy(jp), mState(Waiting), coalition(-1), coolDown(2), offers(vector<int>()) {
    // You can change the implementation of the constructor, but not the signature!
}

//destructor
Party::~Party() {
    if (mJoinPolicy)
        delete mJoinPolicy;
}

//copy constructor
Party::Party(const Party &other) : mId(other.mId), mName(other.mName), mMandates(other.mMandates),
                                   mJoinPolicy(other.mJoinPolicy), mState(other.mState), coalition(other.coalition), coolDown(other.coolDown), offers(other.offers){
    mId = other.mId;
    mName = other.mName;
    mMandates = other.mMandates;
    mState = other.getState();
    coalition = other.coalition;
    coolDown = other.coolDown;
    if (other.mJoinPolicy->getType() == "M") {
        mJoinPolicy = new MandatesJoinPolicy;
    } else {
        mJoinPolicy = new LastOfferJoinPolicy;
    }
}

//assignment operator
Party &Party::operator=(const Party &other) {
    if (this != &other) {
        if (mJoinPolicy)
            delete mJoinPolicy;
        mId = other.mId;
        mName = other.mName;
        mMandates = other.mMandates;
        mState = other.getState();
        coalition = other.coalition;
        coolDown = other.coolDown;
        if (other.mJoinPolicy->getType() == "M") {
            mJoinPolicy = new MandatesJoinPolicy;
        } else {
            mJoinPolicy = new LastOfferJoinPolicy;
        }
    }
    return *this;//
}

State Party::getState() const {
    return mState;
}

void Party::setState(State state) {
    mState = state;
}

int Party::getMandates() const {
    return mMandates;
}

const string &Party::getName() const {
    return mName;
}

void Party::step(Simulation &s) {
    if (getCoolDown() == 0) {
        mJoinPolicy->join(s, offers, mId);
    }
    if ((getCoolDown() != -1) && getState() == State::CollectingOffers) {
        coolDown--;
    }
}

void Party::setCoalition(int coalitionNum) {
    coalition = coalitionNum;
}

int Party::getCoalition() const {
    return coalition;
}

int Party::getCoolDown() const {
    return coolDown;
}

void Party::setCoolDown(int newCoolDown) {
    coolDown = newCoolDown;
}



std::vector<int>& Party::getOffers(){
    return offers;
}
