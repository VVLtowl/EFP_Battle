#include "main.h"
#include "BehaviourFactory.h"

std::list<Behaviour*> BehaviourFactory::Behaviours;

void BehaviourFactory::Register(Behaviour* bh)
{
	Behaviours.emplace_back(bh);
}

void BehaviourFactory::Quit(Behaviour* bh)
{
	Behaviours.remove(bh);
}

void BehaviourFactory::Clear()
{
	while(!Behaviours.empty())
	{
		delete Behaviours.back();
	}
	Behaviours.clear();
	DebugInfo::Print("behaviours size : " +
		std::to_string(Behaviours.size()));
}
