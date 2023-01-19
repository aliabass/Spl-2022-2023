#include "../include/ConnectionHandler.h"
#include "../include/StompProtocol.h"
void readFromSocket(ConnectionHandler *c, StompProtocol *protcol)
{
    while (!protcol->disconnect)
    {
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
        if (protcol->disconnect)
        {
            break;
        }
        std::string line(buf);
        std::vector<std::string> toSend = protcol->process(line);
        for (std::vector<Event>::size_type i = 0; i < toSend.size(); i++)
        {
            c->sendLine(toSend.at(i));
        }
    }
}

int main(int argc, char *argv[])
{
    while (1)
    {
        StompProtocol *protcol = new StompProtocol();
        ConnectionHandler *connction;
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
        std::string line(buf);
        std::vector<std::string> c = protcol->split(line, ' ');
        std::string host = (protcol->split(c.at(1), ':')).at(0);
        short port = std::stoi((protcol->split(c.at(1), ':')).at(1));
        connction = new ConnectionHandler(host, port);
        if (!connction->connect())
        {
            std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
            return 1;
        }
        std::vector<std::string> toSend = protcol->process(line);
        for (std::vector<std::string>::size_type i = 0; i < toSend.size(); i++)
        {
            connction->sendLine(toSend.at(0));
        }
        std::string answer;
        connction->getLine(answer);
        std::string recived = protcol->processOut(answer);
        std::cout << recived << std::endl;
        std::thread t(readFromSocket, connction, protcol);
        while (!protcol->disconnect)
        {
            std::string answer;
            connction->getLine(answer);
            std::string recived = protcol->processOut(answer);
            if (recived.length() != 0)
            {
                std::cout << recived << std::endl;
                if (recived == "Disconnect")
                {
                    break;
                }
            }
        }
        t.join();
        delete protcol;
        delete connction;
    }
    return 0;
}
