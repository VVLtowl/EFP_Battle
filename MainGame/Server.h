#pragma once
#include "Executor.h"
#include "Singleton.h"

#include <list>
#include <string>

//for server keep clients
class ClientMember
{
public:
    SOCKADDR_IN Address;
    std::string Name;
    int TCPSocketID;//just for tcp
    bool Ready;
    //int ID;//need server check//give up, all use TCPSocketID

public:
    ClientMember(
        const SOCKADDR_IN& addr,
        const std::string& name);

    ClientMember(
        const int tcpSockID,
        const std::string& name);
};


class Server :
    public Singleton<Server>,
    public Executor
{
public:
    Server();
    ~Server();
    std::string Name() override { return "server"; };

    /*********************************************************
    * @brief    game room data
    ********************************************************/
public:
    int m_JoinedClientNum=0;
    int m_TargetClientNum=0;

    //for input
    int m_Port;

    //for confirm game room setting button
    bool m_OpenGameRoom = false;

    //for reset game room button
    bool m_ResetGameRoom = false;

    //for start game button
    bool m_StartGame = false;

    /*********************************************************
    * @brief    manage client member
    ********************************************************/
public:
    //for broadcast, manage clients connect
    std::unordered_map<int, ClientMember*> m_ClientMembers;
public:
    void RegisterClientMemberAndSetID(int id,ClientMember* c);
    void QuitClientMember(ClientMember* c);
    void QuitClientMember(int id);
    void ClearClientMembers();
    
    /*********************************************************
    * @brief    state
    ********************************************************/
    int State;
    enum class State_WaitClientsJoin
    {
        WAIT_CLIENT_REQUEST,
        BACK_TO_INPUT_ROOM,
        FINISH_WAIT,

        NONE,
    };


    /*********************************************************
    * @brief    çsà◊
    ********************************************************/
public:
    void StartInWaitRoomScene();
    void StartInputGameRoom();
    void StartWaitClientsJoin();
    void StartCheckClientJoin(int tcpID,std::string newClientName);
    void SetClientDisconnect(int id);
    void SetClientReady(int id, bool ready);

    class ServerInit* BH_Init;
    class ServerUninit* BH_Uninit;
    class ServerInputGameRoom* BH_InputGameRoom;
    class ServerWaitClientsJoin* BH_WaitClientsJoin;
};


