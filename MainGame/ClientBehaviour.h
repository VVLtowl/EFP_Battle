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

	void Start()override;
	void Update()override;
	void End()override;

public:
	bool JoinServer = false;
};

//jump
//once
class ClientRequestJoinServer :
	public ClientBehaviour
{
public:
	ClientRequestJoinServer(class Client* c) :ClientBehaviour(c, true) {};
	std::string Name() override { return "ClientRequestJoinServer"; };

	void Start()override;
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
	void Reset(int targetNum, std::function<void()> endEvent = []() {; });
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
	void Reset(int stepType, std::function<void()> endEvent = []() {; });
};

