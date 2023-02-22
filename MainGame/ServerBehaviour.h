#pragma once
#include "Behaviour.h"

class ServerBehaviour :
	public Behaviour
{
public:
	ServerBehaviour(class Server* s,bool isOnce=false);

protected:
	class Server* m_OwnerServer;
};


class ServerInit :
	public ServerBehaviour
{
public:
	ServerInit(class Server* s) :ServerBehaviour(s) {};
	std::string Name() override { return "ServerInit"; };

	void Start()override;
};

class ServerUninit :
	public ServerBehaviour
{
public:
	ServerUninit(class Server* s) :ServerBehaviour(s) {};
	std::string Name() override { return "ServerUninit"; };

	void Start()override;
};

class ServerInputGameRoom :
	public ServerBehaviour
{
public:
	ServerInputGameRoom(class Server* s) :ServerBehaviour(s) {};
	std::string Name() override { return "ServerInputGameRoom"; };
	
	void Start()override;
	void Update()override;
};

class ServerWaitClientsJoin :
	public ServerBehaviour
{
public:
	ServerWaitClientsJoin(class Server* s) :ServerBehaviour(s) {};
	std::string Name() override { return "ServerWaitClientsJoin"; };

	void Start()override;
	void Update()override;
};

//jump
//once
class ServerCheckClientJoin :
	public ServerBehaviour
{
public:

	ServerCheckClientJoin(
		class Server* s,
		const int tcpSockID,
		const std::string memName);
	ServerCheckClientJoin(
		class Server* s,
		const SOCKADDR_IN& memAddr,
		const std::string memName);
	std::string Name() override { return "ServerCheckClientJoin"; };

	void Start()override;
public:
	class ClientMember* m_CheckMember = nullptr;

};

