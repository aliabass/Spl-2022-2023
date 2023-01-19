#pragma once
#include <map>
#include <iostream>
#include <vector>
#include <string>
#include "../include/StompProtocol.h"
#include "../include/ConnectionHandler.h"
#include "../include/event.h"

class StompProtocol {
private:
    std::map<std::string, std::map<std::string, int>> subIdHashMap;//map<username,map<topic,subId>>
    std::map<std::string, std::map<std::string, std::vector<Event>>> eventsHashMap;//map<username,map<topic,vector<events>>>
    std::map<int, std::pair<std::string, bool>> receiptTopicHashMap;//map<receiptId, <topic,subscribedOrNot>>
    std::map<int, std::string> receiptUsernameHashMap;//map<receiptId,username>
    std::map<int, int> receiptSubIdHashMap;//map<receiptId,subscriptionId>
    int subscriptionId;
    int receiptId;
    std::string loggedInUserName;
    

public:
    StompProtocol();
    bool disconnect;
    std::vector<std::string> split(std::string &line ,char delimiter);

    std::vector<std::string> process(std::string &line);
    //std::string getFrameToSend(std::string &line);
    std::string loginFrameToSend(std::vector<std::string> frame);
    std::string joinFrameToSend(std::vector<std::string> frame);
    std::string exitFrameToSend(std::vector<std::string> frame);
    std::vector<std::string> reportFrameToSend(std::vector<std::string> frame);
    std::string logoutCommand(std::vector<std::string> frame);
    void summaryCommand(std::vector<std::string> frame);


    //std::vector<std::string> split(const std::string& line);
    std::string convertHashMapToString (std::map<std::string, std::string> Event);

    std::string processOut(std::string &line);
    void receivedMessageCommand(std::vector<std::string> frame);
    std::string receivedJoinCommand(std::vector<std::string> frame);
    std::string receivedExitCommand(std::vector<std::string> frame);
    std::string receivedLogoutCommand(std::vector<std::string> frame);


};