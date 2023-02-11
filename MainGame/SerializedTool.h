#pragma once
#include <string>

struct CommonStruct {
    long len;
    void* buff;
};

char* EncodeCommonStruct(const CommonStruct& commonStruct, int& outLen);
bool DecodeCommonStruct(const char* strBuff, long maxLen, CommonStruct& outCommonStruct);


//common msg struct
//totalLen=4+4+?
const size_t LEN_MSG_CONTENT = 4;
struct MsgContent
{
    enum ToExecuteBehaviourID
    {
        NONE,

        //TEST_SEND,
        TEST_SERVER_BROADCAST,
        TEST_SHOW_MSG,

        SERVER_CHECK_CLIENT_JOIN,
        SERVER_SET_CLIENT_DISCONNECT,
        //SERVER_NORTIFY_NEW_CLIENT_JOIN,
        JUDGEMENT_COUNT_PLAYER_FINISHED,
        JUDGEMENT_COUNT_PIECE_FINISHED,
        JUDGEMENT_COMMAND_SET_HAND,
        JUDGEMENT_COMMAND_PIECE_FINISH,
        JUDGEMENT_CHECK_PIECE_MOVE,
        JUDGEMENT_CHECK_PIECE_ESCAPE,
        JUDGEMENT_CHECK_PIECE_CAUGHT,

        //CLIENT_REQUEST_JOIN_SERVER,
        CLIENT_WAIT_TO_JOIN,
        CLIENT_JOIN_ROOM,
        CLIENT_UPDATE_JOINEDNUM,
        CLIENT_DISCONNECT,

        CLIENT_CREATE_PLAYER,
        CLIENT_CREATE_PIECE,
        CLIENT_SET_PIECE_NUM,
        
        CLIENT_SHOW_PIECE_ENTRY,
        CLIENT_SHOW_STEP,
        CLIENT_INPUT_RPS,
        CLIENT_SET_PIECE_HAND,
        CLIENT_SHOW_PIECE_HAND,
        CLIENT_SHOW_CHECK_ACTPOINT,
        CLIENT_INPUT_PIECE_ACT,
        CLIENT_SHOW_PIECE_FINISH,
        CLIENT_SHOW_PIECE_MOVE,
        CLIENT_SHOW_PIECE_ESCAPE,
        CLIENT_SHOW_PIECE_CAUGHT,

        CLIENT_CLEAR_PIECE_HAND_AND_ACTPOINT,

        CLIENT_LOAD_GAMESCENE,
    };

    int BHID = NONE;
    long DataLen;
    void* Data;
};

//net msg buf len
const int LEN_MSG_TOOL = 1024;

//len+=0
const size_t LEN_ZERO = 0;

//len+=512
const size_t LEN_CLIENT_NAME = 64;
const size_t LEN_CLIENT_IP = 64;
struct Data_Client
{
    char Name[LEN_CLIENT_NAME];
    char IP[LEN_CLIENT_IP];
};
const size_t LEN_DATA_CLIENT = sizeof(Data_Client);

//len+=256
struct Data_Char256
{
    char Sentence[256] = "test msg";
};
const size_t LEN_DATA_CHAR256 = sizeof(Data_Char256);

//lent+=4
struct Data_Int
{
    int Data;
};
const size_t LEN_DATA_INT = sizeof(Data_Int);

//len+=16
struct Data_Int4
{
    int Data[4];
};
const size_t LEN_DATA_INT4 = sizeof(Data_Int4);

//len+=16
typedef PlayerDesc Data_PlayerDesc ;
const size_t LEN_DATA_PLAYERDESC = sizeof(Data_PlayerDesc);


//len+=16
typedef PieceDesc Data_PieceDesc;
const size_t LEN_DATA_PIECEDESC = sizeof(Data_PieceDesc);

char* EncodeMsgContent(const MsgContent& msg, char* outBuf=nullptr, int msglen= LEN_MSG_TOOL);
bool DecodeMsgContent(const char* strBuff, MsgContent& outMsg);
