#pragma once
#include "Behaviour.h"

class ServerBehaviour :
	public Behaviour
{
public:
	ServerBehaviour(class AppServer* s,bool isOnce=false);

protected:
	class AppServer* m_AppServer;
};

class ServerInputGameRoom :
	public ServerBehaviour
{
public:
	ServerInputGameRoom(class AppServer* s) :ServerBehaviour(s) {};
	std::string Name() override { return "ServerInputGameRoom"; };
	
	void Start()override;
	void Update()override;
};

class ServerWaitClientsJoin :
	public ServerBehaviour
{
public:
	ServerWaitClientsJoin(class AppServer* s) :ServerBehaviour(s) {};
	std::string Name() override { return "ServerWaitClientsJoin"; };

	void Start()override;
	void Update()override;

public:
	enum class State
	{
		WAIT_CLIENT_REQUEST_OR_RESET,
		BACK_TO_INPUT_ROOM,
		WAIT_SET_CLIENT_CAMPPIECE,
		FINISH_WAIT,

		NONE,
	};
	State m_WaitState = State::NONE;
};

//jump 
//once 
class ServerSetClientCampPiece :
	public ServerBehaviour
{
public:
	ServerSetClientCampPiece(
		class AppServer* s,
		ClientMember* mem);
	std::string Name() override { return "ServerSetClientCampPiece"; };

	void Start()override;
	void Update()override;

private:
	ClientMember* m_SetClient;
	bool m_Finish;
};


//jump
//once
class ServerCheckClientJoin :
	public ServerBehaviour
{
public:
	ServerCheckClientJoin(
		class AppServer* s,
		int memID,
		std::string memName,
		SOCKET* memSock = nullptr,
		SOCKADDR_IN* memAddr = nullptr);
	std::string Name() override { return "ServerCheckClientJoin"; };

	void Start()override;

private:
	ClientMember* m_CheckMember;
};

