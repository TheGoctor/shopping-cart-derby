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
	static CIDGen cIDGen;
	return &cIDGen;
}

unsigned int CIDGen::GetID(string szName)
{
	// See if the name is already defined
	map<string, unsigned int, less<string>, scd::allocator<pair<string, 
		unsigned int>>>::iterator cFoundIter = m_cRegister.find(szName);

	// Return ID if defined
	if(cFoundIter != m_cRegister.end())
		return (*cFoundIter).second;

	// Add string to register if not defined
	std::pair<string, unsigned int> pNewPair(szName, 0);
	m_cRegister.insert(pNewPair);
	int nID = (unsigned int)(*m_cRegister.find(szName)).first.c_str();
	m_cRegister[szName] = nID;

	// Return address of string in map as ID
	// (so that you may cast the ID as a char* to
	// see the string!)
	return nID;
}