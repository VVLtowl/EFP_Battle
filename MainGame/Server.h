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
    int ID;//need server check

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

    /*********************************************************
    * @brief    game room data
    ********************************************************/
public:
    int JoinedClientNum=0;
    int TargetClientNum=0;

    //for input
    int Port;

    //for confirm game room setting button
    bool OpenGameRoom = false;

    //for reset game room button
    bool ResetGameRoom = false;

    //for start game button
    bool StartGame = false;

    /*********************************************************
    * @brief    manage client member
    ********************************************************/
public:
    //for broadcast, manage clients connect
    std::unordered_map<int, ClientMember*> ClientMembers;
    int NewClientJoinID = 0;
public:
    void RegisterClientMemberAndSetID(ClientMember* c);
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
    * @brief    behaviour
    ********************************************************/
public:
    class ServerInit* BH_Init;
    class ServerUninit* BH_Uninit;
    class ServerInputGameRoom* BH_InputGameRoom;
    class ServerWaitClientsJoin* BH_WaitClientsJoin;
};


