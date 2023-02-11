#include "main.h"
#include "Server.h"

#include "ServerBehaviour.h"
#include "NetworkManager.h"

Server::Server()
{
	//reset room data
	JoinedClientNum = 0;
	TargetClientNum = 0;
	
	//reset button
	OpenGameRoom = false;
	ResetGameRoom = false;
	StartGame = false;

	//create behaviour
	BH_Init = new ServerInit(this);
	BH_Uninit = new ServerUninit(this);
	BH_InputGameRoom = new ServerInputGameRoom(this);
	BH_WaitClientsJoin = new ServerWaitClientsJoin(this);
}

Server::~Server()
{
	//delete behaviour by base class destructor
}


#pragma region ========== manage client member ==========

ClientMember::ClientMember(const SOCKADDR_IN& addr, const std::string& name) :
	Name(name)
{
	//constructor set
	memcpy(&Address, &addr, LEN_ADDRIN);

	//wait to set id
	ID = -1;

	//useless in udp
	TCPSocketID = -1;
}

ClientMember::ClientMember(const int tcpSockID, const std::string& name):
	TCPSocketID(tcpSockID),
	Name(name)
{
	//wait to set id
	ID = -1;

	//useless in tcp
	memset(&Address, 0, LEN_ADDRIN);
}

void Server::RegisterClientMemberAndSetID(ClientMember* c)
{
	ClientMembers.emplace(NewClientJoinID, c);

	//set client id
	c->ID = NewClientJoinID;

	NewClientJoinID++;
}

void Server::QuitClientMember(ClientMember* c)
{
	NewClientJoinID = c->ID;
	ClientMembers.erase(c->ID);
		
	JoinedClientNum--;
}

void Server::QuitClientMember(int id)
{
	NewClientJoinID = id;
	ClientMembers.erase(id);

	JoinedClientNum--;
}

void Server::ClearClientMembers()
{
	for (auto mem : ClientMembers)
	{
		delete mem.second;
	}
	ClientMembers.clear();
}


#pragma endregion
