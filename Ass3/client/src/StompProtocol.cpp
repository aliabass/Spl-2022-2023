#include "../include/StompProtocol.h"
using std::map;
using std::string;
using std::vector;
#include <fstream>
#include <iostream>

StompProtocol::StompProtocol() : subIdHashMap{}, eventsHashMap{}, receiptTopicHashMap{}, receiptUsernameHashMap{}, receiptSubIdHashMap{}, subscriptionId{}, receiptId{}, loggedInUserName{}, disconnect{}
{
    receiptId = 0;
    subscriptionId = 0;
    loggedInUserName = "";
}

std::vector<std::string> StompProtocol::split(std::string &line, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    while (getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<std::string> StompProtocol::process(std::string &line)
{
    std::vector<std::string> res;
    std::vector<std::string> tokens = split(line, ' ');
    if (tokens.at(0) == "login")
    {
        if (loggedInUserName.length() != 0)
        {
            std::cout << "The client is already logged in, log out before trying again" << std::endl;
            return res;
        }
        res.push_back(loginFrameToSend(tokens));
        return res;
    }
    if (tokens.at(0) == "join")
    {
        res.push_back(joinFrameToSend(tokens));
        return res;
    }
    if (tokens.at(0) == "exit")
    {
        if(exitFrameToSend(tokens).length()!=0){
            res.push_back(exitFrameToSend(tokens));
        }
        return res;
    }
    if (tokens.at(0) == "report")
    {
        res = reportFrameToSend(tokens);
        return res;
    }
    if (tokens.at(0) == "logout")
    {
        res.push_back(logoutCommand(tokens));
        return res;
    }
    if (tokens.at(0) == "summary")
    {
        summaryCommand(tokens);
        return res;
    }
    std::cout << "please enter a valied command" << std::endl;
    return res;
}

std::string StompProtocol::processOut(std::string &line)
{
    std::vector<std::string> tokens = split(line, '\n');
    if (tokens.at(0) == "CONNECTED")
    {
        return "Login successful";
    }
    if (tokens.at(0) == "RECEIPT")
    {
        int rId = std::stoi((split(tokens.at(1), ':')).at(1));
        if (rId < 0)
        {
            return receivedLogoutCommand(tokens);
        }
        if (receiptTopicHashMap[rId].second)
        {
            return receivedJoinCommand(tokens);
        }
        else
        {
            return receivedExitCommand(tokens);
        }
    }
    if (tokens.at(0) == "ERROR")
    {
        std::cout << (split(tokens.at(1), ':')).at(1) << std::endl;
    }
    if (tokens.at(0) == "MESSAGE")
    {
        receivedMessageCommand(tokens);
    }
    return "";
}

std::string StompProtocol::loginFrameToSend(std::vector<std::string> command)
{
    std::string toSend;
    toSend += "CONNECT\naccept-version:1.2\n";
    toSend += "host:" + command[1] + "\nlogin:" + command[2] + "\npasscode:" + command[3] + "\n\n";
    loggedInUserName = command[2];
    return toSend;
};

std::string StompProtocol::joinFrameToSend(std::vector<std::string> command)
{
    std::string toSend;
    toSend += "SUBSCRIBE\ndestination:" + command[1];
    toSend += "\nid:" + std::to_string(subscriptionId) + "\nreceipt:" + std::to_string(receiptId) + "\n\n";
    receiptTopicHashMap.insert(std::make_pair(receiptId, std::make_pair(command[1], true)));
    receiptSubIdHashMap.insert(std::make_pair(receiptId, subscriptionId));

    subscriptionId++;
    receiptId++;
    return toSend;
}

std::string StompProtocol::exitFrameToSend(std::vector<std::string> command)
{
    std::string toSend;
    if (subIdHashMap[loggedInUserName].count(command[1]) == 0)
    {
        std::cout << "User is not subscribed to this topic" << std::endl;
        return "";
    }
    int subId = subIdHashMap[loggedInUserName][command[1]]; // get the id from subIdHashMap
    toSend += "UNSUBSCRIBE\nid:" + std::to_string(subId) + "\nreceipt:" + std::to_string(receiptId) + "\n\n";
    receiptTopicHashMap.insert(std::make_pair(receiptId, std::make_pair(command[1], false)));
    receiptSubIdHashMap.insert(std::make_pair(receiptId, subId));
    // now, when receiving the frame from the server, i have all the info which allow me to delete info from hm1 and hm2
    // then, just if unsubscribed done i will change my maps

    receiptId++;
    return toSend;
}

std::vector<std::string> StompProtocol::reportFrameToSend(std::vector<std::string> command)
{ // in the main to send the events one by one
    std::vector<std::string> allEvents;
    names_and_events parsedFile = parseEventsFile(command[1]);
    std::string gameName = parsedFile.team_a_name + "_" + parsedFile.team_b_name;
    if (subIdHashMap[loggedInUserName].count(gameName) == 0)
    {
        std::cout << "User is not subscribed to this topic" << std::endl;
        return allEvents;
    }
    std::vector<Event> events = parsedFile.events; // each place have specific event
    for (std::vector<Event>::size_type i = 0; i < events.size(); ++i)
    {                         // after moving to all the events, this will give us one report
        std::string tmpEvent; // this will include specific event details
        std::string gameUpdates = convertHashMapToString(events[i].get_game_updates());
        std::string team_a_Updates = convertHashMapToString(events[i].get_team_a_updates());
        std::string team_b_Updates = convertHashMapToString(events[i].get_team_b_updates());

        tmpEvent += "SEND\ndestination:" + gameName + "\n\n" + "user: " + loggedInUserName + "\nteam a: " + events[i].get_team_a_name() + "\nteam b: " + events[i].get_team_b_name() + "\nevent name: " + events[i].get_name() + "\ntime: " + std::to_string(events[i].get_time()) + "\ngeneral game updates:\n" + gameUpdates + "\nteam a updates:\n" + team_a_Updates + "\nteam b updates:\n" + team_b_Updates + "\ndescription:\n" + events[i].get_discription();
        allEvents.push_back(tmpEvent);
    }
    return allEvents;
}

std::string StompProtocol::logoutCommand(std::vector<std::string> frame)
{
    std::string toSend;
    if (receiptId == 0)
    {
        receiptId = 1;
    }
    toSend += "DISCONNECT\nreceipt:" + std::to_string(-receiptId) + "\n\n";
    receiptUsernameHashMap.insert(std::make_pair(receiptId, loggedInUserName));

    receiptId++;
    return toSend;
}

void StompProtocol::summaryCommand(std::vector<std::string> frame)
{
    string toSend = "";
    map<string, string> firstTeamUpdates;
    map<string, string> secondTeamUpdates;
    map<string, string> generalGameUpdates;
    string eventReports; // this will include : time of event, event name, description of the event
    if (subIdHashMap[loggedInUserName].count(frame[1]) == 0)
    {
        std::cout << "User is not subscribed to this topic" << std::endl;
        return;
    }
    // to save the two team names
    int position = (frame[1]).find("_");
    string firstTeamName = frame[1].substr(0, position);
    firstTeamName = char(std::tolower(firstTeamName.at(0))) + firstTeamName.substr(1);
    string secondTeamName = frame[1].substr(position + 1);
    secondTeamName = char(std::tolower(secondTeamName.at(0))) + secondTeamName.substr(1);

    // now moving on all the events
    vector<Event> userEvents = eventsHashMap[frame[2]][frame[1]];

    for (std::vector<Event>::size_type i = 0; i < userEvents.size(); ++i)
    {
        Event currEvent = userEvents[i];

        // prepare the event reports
        int currTime = currEvent.get_time();
        string eventName = currEvent.get_name();
        string description = currEvent.get_discription();
        eventReports += std::to_string(currTime) + " - " + eventName + ":\n\n" + description + "\n\n\n";

        // now to deal with first team updates
        map<string, string> currFirstTeamUpdates = currEvent.get_team_a_updates();
        for (auto it = currFirstTeamUpdates.begin(); it != currFirstTeamUpdates.end(); ++it)
        {
            string key = it->first;
            string value = it->second;
            if (firstTeamUpdates.find(key) != currFirstTeamUpdates.end())
            {
                // key is present in firstTeamUpdates, so we have to update the value
                firstTeamUpdates[key] = it->second;
            }
            else
            { // new update
                firstTeamUpdates.emplace(std::make_pair(key, value));
            }
        }

        // now to deal with second team updates
        map<string, string> currGameUpdates = currEvent.get_game_updates();
        for (auto it = currGameUpdates.begin(); it != currGameUpdates.end(); ++it)
        {
            string key = it->first;
            string value = it->second;
            if (generalGameUpdates.find(key) != currGameUpdates.end())
            {
                // key is present in generalGameUpdates, so we have to update the value
                generalGameUpdates[key] = it->second;
            }
            else
            { // new update
                generalGameUpdates.emplace(std::make_pair(key, value));
            }
        }

        // now to deal with general game updates
        map<string, string> currSecondTeamUpdates = currEvent.get_team_b_updates();
        for (auto it = currSecondTeamUpdates.begin(); it != currSecondTeamUpdates.end(); ++it)
        {
            string key = it->first;
            string value = it->second;
            if (secondTeamUpdates.find(key) != currSecondTeamUpdates.end())
            {
                // key is present in secondTeamUpdates, so we have to update the value
                secondTeamUpdates[key] = it->second;
            }
            else
            { // new update
                secondTeamUpdates.emplace(std::make_pair(key, value));
            }
        }
    }

    // now to convert the hash maps into string
    string teamOneUpdates = convertHashMapToString(firstTeamUpdates);
    string teamTwoUpdates = convertHashMapToString(secondTeamUpdates);
    string gameUpdates = convertHashMapToString(generalGameUpdates);

    // now to prepare the summary:
    if (eventsHashMap[frame[2]][frame[1]].size() != 0)
    {
        toSend += firstTeamName + " vs " + secondTeamName + "\nGame stats:\nGeneral stats:\n" + gameUpdates + firstTeamName + " stats:\n" + teamOneUpdates + secondTeamName + " stats:\n" + teamTwoUpdates + "Game event reports:\n" + eventReports;
    }
    std::ofstream outfile(frame[3]);
    outfile << toSend;
}

// after the sockThread read the reply from the socket (sended by server), we should do the proper changes
// for commands : join, exit, logout.. these commands receive frame from the server
// to call these functions from the main just if i have frame to deal with, like the frames received in the pdf
std::string StompProtocol::receivedJoinCommand(std::vector<std::string> frame)
{
    int receipt = std::stoi(frame[1].substr(11)); // stoi makes the num integer
    string topic = receiptTopicHashMap[receipt].first;
    // string username = receiptUsernameHashMap[receipt];
    int sub_id = receiptSubIdHashMap[receipt];
    // now add to the hash map
    subIdHashMap[loggedInUserName][topic] = sub_id;
    return "Joined channel " + topic;
}

std::string StompProtocol::receivedExitCommand(std::vector<std::string> frame)
{
    int receipt = std::stoi(frame[1].substr(11)); // stoi makes the num integer
    string topic = receiptTopicHashMap[receipt].first;
    subIdHashMap[loggedInUserName].erase(topic);
    return "Exited channel " + topic;
}

std::string StompProtocol::receivedLogoutCommand(std::vector<std::string> frame)
{
    int receipt = std::stoi(frame[1].substr(11)); // stoi makes the num integer
    string username = receiptUsernameHashMap[receipt];
    subIdHashMap.erase(username);
    eventsHashMap.erase(username);
    loggedInUserName = "";
    disconnect = true;
    return "Disconnect";
}

void StompProtocol::receivedMessageCommand(std::vector<std::string> frame)
{
    string username = frame[6].substr(6);
    string team_a_name = frame[7].substr(8);
    string team_b_name = frame[8].substr(8);
    string gameName = team_a_name + "_" + team_b_name;
    string eventName = frame[9].substr(12);
    int eventTime = std::stoi(frame[10].substr(6));

    std::map<std::string, std::string> game_updatesHM;
    std::map<std::string, std::string> team_a_updatesHM;
    std::map<std::string, std::string> team_b_updatesHM;

    // to find the indexes of: team a updates: / team b updates: / description:
    int generalGameUpdatesIndex = 11;

    auto itA = std::find(frame.begin(), frame.end(), "team a updates:");
    int team_a_UpdatesIndex = std::distance(frame.begin(), itA);

    auto itB = std::find(frame.begin(), frame.end(), "team b updates:");
    int team_b_UpdatesIndex = std::distance(frame.begin(), itB);

    auto itDes = std::find(frame.begin(), frame.end(), "description:");
    int descriptionIndex = std::distance(frame.begin(), itDes);

    // the description
    string eventDescription = frame[descriptionIndex + 1];

    // now to put the updates i have in the proper hash maps:
    for (int i = generalGameUpdatesIndex; i < team_a_UpdatesIndex - 2; ++i)
    {
        string currStr = frame[i + 1];               // active: true
        int splitIdx = currStr.find(':');            // 6
        string key = currStr.substr(0, splitIdx);    // active
        string value = currStr.substr(splitIdx + 2); // true
        game_updatesHM.emplace(std::make_pair(key, value));
    }
    //    std::cout<<team_a_UpdatesIndex<<std::endl;
    for (int i = team_a_UpdatesIndex; i < team_b_UpdatesIndex - 2; ++i)
    {
        string currStr = frame[i + 1];
        int splitIdx = currStr.find(':');
        string key = currStr.substr(0, splitIdx);
        string value = currStr.substr(splitIdx + 2);
        team_a_updatesHM.emplace(std::make_pair(key, value));
    }
    //    std::cout<<"end loop"<<std::endl;
    for (int i = team_b_UpdatesIndex; i < descriptionIndex - 2; ++i)
    {
        string currStr = frame[i + 1];
        int splitIdx = currStr.find(':');
        string key = currStr.substr(0, splitIdx);
        string value = currStr.substr(splitIdx + 2);
        team_b_updatesHM.emplace(std::make_pair(key, value));
    }

    // now to initiate the event and then insert to events hash map
    Event receivedEvent(team_a_name, team_b_name, eventName, eventTime,
                        game_updatesHM, team_a_updatesHM, team_b_updatesHM, eventDescription);
    eventsHashMap[username][gameName].push_back(receivedEvent);
    return;
}

std::string StompProtocol::convertHashMapToString(std::map<std::string, std::string> Event)
{
    std::string out;
    for (const auto &kv : Event)
    {
        out += kv.first + ": " + kv.second + "\n";
    }
    return out;
}