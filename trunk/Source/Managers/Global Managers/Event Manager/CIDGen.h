////////////////////////////////////////////////////////////////////////////////
//	File			:	CIDGen.h
//	Date			:	4/4/11
//	Mod. Date		:	4/4/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	Class converts strings into unique IDs and stores the
//						pairs into a map.
////////////////////////////////////////////////////////////////////////////////

#ifndef _CIDGEN_H_
#define _CIDGEN_H_

#include <map>
#include <string>
using std::map;
using std::string;

class CIDGen
{
	map<string, unsigned int> m_cRegister;

	// Constructor
	CIDGen();

	// Trilogy of Evil
	~CIDGen();
	CIDGen(const CIDGen&);
	CIDGen& operator=(const CIDGen&);

public:
	static CIDGen* GetInstance();
	unsigned int GetID(string szName);
};

#endif