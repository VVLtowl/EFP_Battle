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
        SERVER_SET_CLIENT_READY,
        //SERVER_NORTIFY_NEW_CLIENT_JOIN,
        JUDGEMENT_COUNT_PLAYER_FINISHED,
        JUDGEMENT_COUNT_PIECE_FINISHED,
        JUDGEMENT_COMMAND_SET_HAND,
        JUDGEMENT_COMMAND_PIECE_FINISH,
        JUDGEMENT_CHECK_PIECE_MOVE,
        JUDGEMENT_CHECK_PIECE_ESCAPE,
        JUDGEMENT_CHECK_PIECE_CAUGHT,

        //CLIENT_REQUEST_JOIN_SERVER,
        CLIENT_SET_ID,
        CLIENT_WAIT_TO_JOIN,
        CLIENT_JOIN_ROOM,
        CLIENT_JOIN_ROOM_FAIL,
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
        CLIENT_CONTINUE_INPUT_PIECE_MOVE,

        CLIENT_CLEAR_PIECE_HAND_AND_ACTPOINT,
        CLIENT_CLEAR_PIECE_HAND,
        CLIENT_CLEAR_PIECE_ACTPOINT,
        CLIENT_SHOW_GAME_OVER,

        CLIENT_LOAD_GAMESCENE,
    };

    int BHID = NONE;
    long DataLen;
    void* Data;
};

//net msg buf len
const int LEN_MSG_TOOL = 1024;

char* EncodeMsgContent(const MsgContent& msg, char* outBuf=nullptr, int msglen= LEN_MSG_TOOL);
bool DecodeMsgContent(const char* strBuff, MsgContent& outMsg);
