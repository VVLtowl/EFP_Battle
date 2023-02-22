#pragma once
#include "Behaviour.h"

class ClientBehaviour :
	public Behaviour
{
public:
	ClientBehaviour(class Client* c, bool isOnce = false);

protected:
	class Client* m_OwnerClient;
};


class ClientInit :
	public ClientBehaviour
{
public:
	ClientInit(class Client* c) : ClientBehaviour(c) {};
	std::string Name() override { return "ClientInit"; };

	void Start()override;
};

class ClientUninit :
	public ClientBehaviour
{
public:
	ClientUninit(class Client* c) : ClientBehaviour(c) {};
	std::string Name() override { return "ClientUninit"; };

	void Start()override;
};

class ClientSelectServer :
	public ClientBehaviour
{
public:
	ClientSelectServer(class Client* c) :ClientBehaviour(c) {};
	std::string Name() override { return "ClientSelectServer"; };

private:
	void Start()override;
	void Update()override;

public:
	enum class State
	{
		NONE,
		SELECT_SERVER,
		TRY_JOIN,
		WAIT_SERVER_CHECK,
		
		FINISH_JOIN,//result
		FINISH_EXIT,//result
	};
	State m_SelectState = State::NONE;
	bool m_JoinServer = false;
};

class ClientTryConnectServer :
	public ClientBehaviour
{
public:
	ClientTryConnectServer(class Client* c) :ClientBehaviour(c) {};
	std::string Name() override { return "ClientTryConnectServer"; };

	void Reset(std::function<void()> onSuccess = []() {},
		std::function<void()> onFail = []() {},
		DWORD timeOut = 1000);//todo: use select() to set time out 
	void Update()override;

public:
	enum class State
	{
		NONE,
		WAIT_CONNECT,
		
		CHECK_CONNECT,

		CONNECT_SUCCEED,//result
		CONNECT_TIME_OUT,//result
	};
	State m_ConnectState = State::NONE;
	std::function<void()> OnConnectSuccess;
	std::function<void()> OnConnectFail;
private:
	DWORD m_TimeOut = 0;
	DWORD m_TimeCount = 0;
	DWORD m_TimeStamp = 0;
};

//once
class ClientRequestJoinServer :
	public ClientBehaviour
{
public:
	ClientRequestJoinServer(class Client* c) :ClientBehaviour(c,true) {};
	std::string Name() override { return "ClientRequestJoinServer"; };

	void Start()override;
};

class ClientWaitRoom :
	public ClientBehaviour
{
public:
	ClientWaitRoom(class Client* c) :ClientBehaviour(c) {};
	std::string Name() override { return "ClientWaitRoom"; };

private:
	void Start()override;
	void Update()override;

public:
	enum class State
	{
		NONE,

		INPUT_READY,
		WAIT_START,

		FINISH,
	};
	State m_WaitState = State::NONE;
	bool m_Ready = false;
};

class ClientWaitPiecesFinish :
	public ClientBehaviour
{
public:
	ClientWaitPiecesFinish(class Client* c) :
		ClientBehaviour(c) {};
	std::string Name() override { return "ClientWaitPiecesFinish"; };

	void Update()override;

public:
	bool Working=false;//flag for execute once
	int PrintRowID = -1;

	int FinishPieceNum = 0;
	int TargetPieceNum = 0;
	void Reset(int targetNum,std::string endIndo ,std::function<void()> endEvent = []() {; });
	void PrintCount(std::string info);
};

class ClientShowStep :
	public ClientBehaviour
{
public:
	ClientShowStep(class Client* c);
	~ClientShowStep();
	std::string Name() override { return "ClientShowStep"; };

	void Start()override;
	void Update()override;

public:
	class ShowUIHelper* ShowHelper;

public:
	void Reset(int stepType, std::string endInfo, std::function<void()> endEvent = []() {; });
};

class ClientShowGameOver :
	public ClientBehaviour
{
public:
	ClientShowGameOver(class Client* c);
	~ClientShowGameOver();
	std::string Name() override { return "ClientShowGameOver"; };

	void Start()override;
	void Update()override;

public:
	class ShowGameOverUIHelper* m_ShowHelper;

public:
	enum class ResultType
	{
		BAD_WIN,
		GOOD_WIN,
	};
	void Reset(ResultType result,std::string endInfo, std::function<void()> endEvent = []() {; });
};


