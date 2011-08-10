////////////////////////////////////////////////////////////////////////////////
//	File			:	CIDGen.h
//	Date			:	4/4/11
//	Mod. Date		:	7/26/11
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

#include "../Memory Manager/CAllocator.h"

class CIDGen
{
	map<string, unsigned int, less<string>, CAllocator<
		pair<string, unsigned int>>> m_cRegister;

	// Constructor
	CIDGen();

	// Trilogy of Evil
	~CIDGen();
	CIDGen(const CIDGen&);
	CIDGen& operator=(const CIDGen&);

public:
	static CIDGen* GetInstance();

	////////////////////////////////////////////////////////////////////////////////
	// Function: “GetID”
	//
	// Return:  unsigned int	-	The ID of the string and the address of where
	//								the string is stored in memory.
	//
	// Parameters: string szName	-	The string to be converted into an ID
	//
	// Purpose:  This function converts the passed in string into an ID by storing
	//			 it into a container and returning the address of the string after
	//			 it has been stored, guaranteeing a unique ID for each unique string
	////////////////////////////////////////////////////////////////////////////////
	unsigned int GetID(string szName);
};

#endif