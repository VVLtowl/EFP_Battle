#pragma once
#include "Singleton.h"
#include "RPS.h"
#include <queue>
//#pragma pack(8) //x86 default

//net package definition
const unsigned short LEN_MSG = 1024;

//sockaddr_in size
const size_t LEN_ADDRIN = sizeof(SOCKADDR_IN);

//tcp socket max num
const size_t MAX_TCPSOCK = 10;

//check tcp socket usable
struct TCPSocket
{
    bool IsUsed = false;
    SOCKET Socket;
};


//for sendto
class SendMsg
{
public:
    SendMsg(const char* msg);//for tcp client side
    SendMsg(int tcpSockID, const char* msg);
    SendMsg(const SOCKADDR_IN& addrin, const char* msg);
public:
    int TCPSockID = -1;
    SOCKADDR_IN Destination;
    char MsgBuffer[LEN_MSG];

    //for debuginfo print
public:
    std::string Comment;
    int PrintRowID = -1;
};

//for recvfrom
class RecvMsg
{
public:
    RecvMsg(const char* msg);//for tcp client
    RecvMsg(const int tcpSockID,
        const char* msg);
    RecvMsg(const SOCKADDR_IN& addrin,
        const char* msg);
public:
    int TCPSockID = -1;
    SOCKADDR_IN From;
    char MsgBuffer[LEN_MSG];
};

class NetworkManager :
    public Singleton<NetworkManager>
{
public:
    bool Running = false;
    bool IsTCP = false;

public:
    WSADATA WsaData;
    SOCKET Socket;//for udp or client socket
    SOCKET TCPListenSocket;//for tcp listen accept
    TCPSocket TCPSockets[MAX_TCPSOCK];//for tcp
    int GetEnableTCPID();
    int Port;
    char HostIP[64];
    HOSTENT* HostEnt;

public:
    class Server* TargetServer = nullptr;
    class Client* TargetClient = nullptr;

public:
    std::queue<RecvMsg*> RecvMsgQueue;
    std::queue<SendMsg*> SendMsgQueue;

public:
    //if server
    void StartAsServer();
    void TryLaunchServer(int port, bool isTcp);

    //if client
    void StartAsClient();
    void SetConnectServerInfo(int port, std::string serverIP, bool isTcp);

    //common
    void InitChatPanel();
    void Close();


    /*********************************************************
    * @brief    send msg
    ********************************************************/
    void Client_SendTestMsgToBroadcast(const char* sentence);
    void Server_BroadcastTestMsgToShow(const char* sentence);

    void Client_RequestDisconnect();
    void Server_CommandDisconnect();

    void Client_RequestJoinServer();
    void Client_TryConnectServer();
    void Server_AgreeJoin(class ClientMember* member);
    void Server_CommandUpdateJoinedNum();


    void Server_CommandLoadGameScene();

    void Judgement_CommandCreatePlayer();
    void Judgement_CommandSetPieceNum();
    void Judgement_CommandCreatePiece(class Piece* piece);
    void Judgement_CommandPieceShowEntry(class Piece* piece);
    void Judgement_CommandShowStep(int showStepType);
    void Judgement_CommandInputRPS();
    void Judgement_CommandShowPieceHand(class Piece* piece);
    void Judgement_CommandPieceShowCheckActpoint(class Piece* piece);
    void Judgement_CommandPieceInputAct(class Piece* piece);

    void Client_NotifyCountPlayerFinished();
    void Client_NotifyCountPieceFinished();//give up
    
    void Piece_RequestSetHand(int pieceID, HandType handType);
    void Judgement_CommandSetHand(int pieceID, HandType handType);

    void Piece_RequestMoveAct(int pieceID, int squareID);
    void Piece_RequestFinishAct(int pieceID);
    void Judgement_CommandPieceFinishAct(int pieceID);
    void Judgement_CommandPieceMove(int pieceID, int squareID);
    void Judgement_CommandPieceCaught(int pieceID);
    void Judgement_CommandPieceEscape(int pieceID);
    void Judgement_CommandPieceContinueMove(int clientID, int pieceID);

    void Judgement_CommandPiecesClearHandAndActpoint();

    //execute on multi thread
    /*********************************************************
    * @brief
    ********************************************************/
    //bool ThreadRunning;
    //void CheckRecvMsg();
    //void CheckSendMsg();



    /*********************************************************
    * @brief    for communication
    ********************************************************/
public:
    void TCPMsgRecv(int tcpSockID, LPARAM lp);
    void SentToAllClients(const char* msgBuf, const std::string& comment);
    void UpdateNetProc(UINT uMsg, LPARAM lp);
    void Update();
};

