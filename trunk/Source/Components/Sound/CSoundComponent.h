///////////////////////////////////////////////////////////////////////////////
//  File			:	"CSoundComponent.h"
//
//  Author			:	Huy Nguyen (HN)
//
//	Team			:	Falcon Fish Productions
//
//  Date Created	:	4/14/2011
//
//	Last Changed	:	4/14/2011
//
//  Purpose			:	
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _CSOUNDCOMPONENT_H_
#define _CSOUNDCOMPONENT_H_

#include <windows.h>
#define LEAN_AND_MEAN

#include"..\\..\\IComponent.h"

class CObject;
class CSoundManager;

class CSoundComponent : public IComponent
{
private:
	CSoundManager*	m_pSM;			//Pointer to the SoundManager
	CObject*		m_pcParent;		//The Game object this Comp is assoicated with
	bool			m_bIsPlaying;	//A bool if the SFX/Music is playing

public:






};
#endif //_CSOUNDCOMPONENT_H_