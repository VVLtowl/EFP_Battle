#include "imgui/imgui.h"
#include "debugInfo.h"





std::string DebugInfo::m_Info[BDEBUG_INFO_MAX];
std::string DebugInfo::m_NetMsg[BDEBUG_INFO_MAX];
std::unordered_map<int, std::function<void()>> DebugInfo::TestBlocks;
bool DebugInfo::m_NeedClear;
static int m_id;
static int m_idNetMsg;


void DebugInfo::CloseBlock(int id)
{
	TestBlocks.erase(id);
}

void DebugInfo::Update()
{
	ImGui::Begin("Debug Info Window"); 
	if (ImGui::Button("Clear"))
	{
		int id = 0;
		m_id = 0;
		while (id < BDEBUG_INFO_MAX)
		{
			m_Info[id] = ""; 
			id++;
		}
	}
	int id = 0;
	while(id < BDEBUG_INFO_MAX)
	{
		ImGui::Text("%s\n", m_Info[id].c_str());
		id++;
	}
	ImGui::End();

	for (auto block : TestBlocks)
	{
		block.second();
	}
}


const int DebugInfo::Print(std::string info)
{
	int out = m_id;
	m_Info[m_id] = info;
	int next = (m_id + 1) % BDEBUG_INFO_MAX;
	m_Info[next] = "----------------------";

	m_id = (m_id + 1) % BDEBUG_INFO_MAX;
	return out;
}

void DebugInfo::PrintRow(int rowID, std::string info)
{
	m_Info[rowID] = info;
}


void DebugInfo::DrawNetMsg()
{
	ImGui::Begin("Room Chat");

	int id = 0;
	if (ImGui::Button("Clear"))
	{
		m_id = 0;
		while (id < BDEBUG_INFO_MAX)
		{
			m_NetMsg[id] = "";
			id++;
		}
	}

	id = 0;
	while (id < BDEBUG_INFO_MAX)
	{
		ImGui::Text("%s\n", m_NetMsg[id].c_str());
		id++;
	}
	ImGui::End();
}

const int DebugInfo::PrintNetMsg(std::string info)
{
	int out = m_idNetMsg;
	m_NetMsg[m_idNetMsg] = info;
	int next = (m_idNetMsg + 1) % BDEBUG_INFO_MAX;
	m_NetMsg[next] = "----------------------";

	m_idNetMsg = (m_idNetMsg + 1) % BDEBUG_INFO_MAX;
	return out;
}


int DebugInfo::WriteInfoToLog(std::string fileName)
{
	FILE* log;
	fopen_s(&log, fileName.c_str(), "w");
	if (log == NULL) {
		//OutputDebugString("Log file open failed.");
		return -1;
	}

	int id = 0;
	while (id < BDEBUG_INFO_MAX)
	{
		fprintf(log, "%s\n", m_Info[id].c_str());
		id++;
	}
	fclose(log);
	return 0;
}
