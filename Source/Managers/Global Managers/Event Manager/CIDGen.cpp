////////////////////////////////////////////////////////////////////////////////
//	File			:	CIDGen.cpp
//	Date			:	4/4/11
//	Mod. Date		:	4/4/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	Class converts strings into unique IDs and stores the
//						pairs into a map.
////////////////////////////////////////////////////////////////////////////////

#include "CIDGen.h"

CIDGen::CIDGen()
{
}

CIDGen::~CIDGen()
{
}

CIDGen* CIDGen::GetInstance()
{
	static CIDGen m_cInstance;
	return &m_cInstance;
}

unsigned int CIDGen::GetID(string szName)
{
	map<string, unsigned int>::iterator cFoundIter = m_cRegister.find(szName);
	if(cFoundIter != m_cRegister.end())
		return (*cFoundIter).second;

	std::pair<string, unsigned int> pNewPair(szName, 0);
	m_cRegister.insert(pNewPair);
	int nID = (unsigned int)(*m_cRegister.find(szName)).first.c_str();
	m_cRegister[szName] = nID;

	return nID;
}