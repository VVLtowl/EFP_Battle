#pragma once
#include "Executor.h"
#include "Singleton.h"
#include "network/NetworkObject.h"

#include <list>
#include <string>

struct ClientMember
{
    int ID;
    std::string Name;
    bool Ready;
    SOCKET* TCPSock;
    SOCKADDR_IN* UDPAddr;

    //disconnect flag
    bool Disconnect = false;

    //for set camp and piece
    bool SetCampPiece = false;
    CampType Camp=CampType::NONE;
    CharacterType SelfPiece[4] = { CharacterType::NONE };
};

class AppServer :
    public Singleton<AppServer>,
    public Executor
{
public:
    AppServer();
    ~AppServer();
    std::string Name() override { return "app server"; };

    /*********************************************************
    * @brief    server network object
    ********************************************************/
public:
    class MyServer* m_MyServer = nullptr;

    /*********************************************************
    * @brief    game room data
    ********************************************************/
public:
    //client member
    std::unordered_map<int, ClientMember*> m_ClientMembers;
    int m_JoinedClientNum = 0;
    int m_TargetClientNum = 0;

    //for input
    int m_Port;

    //for confirm game room setting button
    bool m_OpenGameRoom = false;

    //for reset game room button
    bool m_ResetGameRoom = false;

    //for start game button
    bool m_StartGame = false;


    /*********************************************************
    * @brief    çsà◊
    ********************************************************/
public:
    void Init();
    void Uninit();
    void ClearMember();

    void StartInWaitRoomScene();

    void StartInputGameRoom();
    void LaunchMyServer();

    void StartWaitClientsJoin();
    void StartCheckClientJoin(int tcpID,std::string newClientName);
    void SetClientDisconnect(int id);
    void SetClientReady(int id, bool ready);

    class ServerInputGameRoom* BH_InputGameRoom;
    class ServerWaitClientsJoin* BH_WaitClientsJoin;
};


