#include "../../includes/Server.hpp"

void Server::sendMessage(int fd, std::string message)
{
    if (send(fd, message.c_str(), message.size(), 0) == -1)
    {
        std::cerr << "Failed to sendMessage welcome message to client." << std::endl;
    }
}

std::string Server::buildWelcomeMessage(Channel &channel)
{
    std::string welcomeMessage = "Welcome to the channel " + channel.getChannelName() + "!\n";
    welcomeMessage += "Channel Topic: " + channel.getTopic() + "\n";
    std::string membersList = "Channel Members: ";
    std::map<int, Client*> members = channel.getMembers();
    std::map<int, Client*> operators = channel.getOperators();

    for (std::map<int, Client*>::const_iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt)
	{
        Client* memberClient = memberIt->second;
        if (memberClient) {
            membersList += memberClient->getNickName() + ", ";
        }
    }
    membersList += "\n";
    welcomeMessage += membersList;
    return welcomeMessage;
}

void Server::printParams() const
{
    std::cout << "Command: " << _cmd << "\n";
    std::cout << "Parameters:\n";
    for (size_t i = 0; i < _params.size(); ++i) {
        std::cout << "Param " << i + 1 << ": " << _params[i] << "\n";
    }
}
