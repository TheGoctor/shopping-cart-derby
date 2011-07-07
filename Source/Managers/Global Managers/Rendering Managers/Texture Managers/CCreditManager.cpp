#include "CCreditManager.h"
#include "..\..\Event Manager\CEventManager.h"
#include "..\..\Event Manager\CIDGen.h"
#include "..\..\Event Manager\EventStructs.h"
#include "..\..\..\..\CObject.h"
#include "CBitmapFont.h"
#include "CTextureManager.h"
#include "..\..\Object Manager\CObjectManager.h"
//#include "..\Button\CButtonComponent.h"
using namespace EventStructs;


CCreditManager::CCreditManager(void) 
{
	m_pTM = CTextureManager::GetInstance();
	m_pOM = CObjectManager::GetInstance();
	m_pBMFont = NULL;
	m_pCredits	= NULL;
	m_fSeconds = NULL;
	m_fDT = 0.0f;
	m_pCreditObj = NULL;

}
CCreditManager::~CCreditManager(void)
{
}
void CCreditManager::DisableCredits(IEvent* /*pEvent*/, IComponent* /*pComp*/)
{
	GetInstance()->Shutdown();
}
void CCreditManager::Init(void)
{
	m_pTM = CTextureManager::GetInstance();
	m_pOM = CObjectManager::GetInstance();
	//GetInstance()->m_szCredits =
//"SHOPPING CART DERBY\n\nART LEAD:\nWILLIAM N. PAGE III\n\nENVIRONMENT LEAD:
	/*\nADAM HOSTETLER\n\nCHARACTER/\nPROP ARTIST:\nDOMINIC DUBLIKAR\n\nEFFECTS LEAD:
	\nDAVID GRUNZWEIG\n\n2D ART LEAD: \nJOSEPH RICO\n\n\nTech LEAD:
	\nMACKENZIE REICHELT\n\nJOE-OH-OH-AH-AH-AH\n-AAAA-HA-AH-AH-AH:
	\nJOSEPH LEYBOVICH\n\nQUALITY ASSURANCE LEAD:\nMALCOLM SMITH\n\nGIANT MAN:
	\nRAY NIEVES\n\nSOUND LEAD: \nHUY NGUYEN\n\nSCALE ROTATE TRANSLATE: \nLANDON BARROW\n
		\nAI LEAD: \nJESSE STANCIU\n\n\nSPECIAL THANKS:\n\nGERMINAL ROSELL\n\nJOHN BERTOLINI\n
		\nDERRICK HUGHES\n\nZACHARY HELMS\n\nJEREMIAH BLANCHARD\n\nPEDRO BONILLA\n
		\nALVIN HUNG\n\nMATT TARDIFF\n\nDR RICHARD BAHIN\n\nROB MUSCARELLA\n\nMATT NORRIS\n
		\nCHUCK RAINEY\n\nWENDY JONES\n\nJOE WINTER\n\nENDGAME STUDIOS\n\nAIRRUN STAHL\n\nJEFF PARROT\n
		\nDUY NGUYEN\n\nAND OUR FAMILIES\nAND FRIENDS!";*/

	//register for them event thingamawhats
	CEventManager * pEM = CEventManager::GetInstance();
	int eAssState = CREDITS_STATE;
	string szEvent = "Update";
	szEvent += (char)eAssState;
	pEM->RegisterEvent(szEvent, (IComponent*)GetInstance(), UpdateCallback);
	szEvent = "InitObjects";
	szEvent += CREDITS_STATE;
	pEM->RegisterEvent(szEvent, (IComponent*)GetInstance(), InitCredits);
	szEvent = "DisableObjects";
	szEvent+= CREDITS_STATE;
	pEM->RegisterEvent(szEvent, (IComponent*)GetInstance(), DisableCredits);
	
}
void CCreditManager::InitCredits(IEvent* /*pEvent*/, IComponent* /*pComp*/)
{
	//set up the text
	//int nCredFontID = GetInstance()->m_pTM->LoadTexture("Resource\\BitmapFont.png");
	//GetInstance()->m_pCreditObj = GetInstance()->m_pOM->CreateObject("CreditText");
	//TSpriteData tBMF;
	//tBMF.m_nTexture = nCredFontID;
	//GetInstance()->m_pBMFont = GetInstance()->m_pTM->CreateSpriteComp(
	//	GetInstance()->m_pCreditObj, tBMF, false);
	GetInstance()->m_pCreditObj2 = CObjectManager::GetInstance()->CreateObject("CreditObj");
	//CBitmapFont cFont(-1, 13, 15, 33);
	//cFont.LoadFont("Resource\\BitmapFont.png", "Resource\\BitmapFont_Width.bin");
	//GetInstance()->m_pCredits = GetInstance()->m_pTM->CreateBitmapFontComp(GetInstance()->m_pCreditObj2,
	//	GetInstance()->m_szCredits, cFont, 280/*258*/, 1168, 
	//	1.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f), true);
	//GetInstance()->m_pCredits->SetWord(GetInstance()->m_szCredits);
	//GetInstance()->fX = 280;//258;
	//GetInstance()->fY = 1168;
	
	//make the receipt paper
	GetInstance()->pButton = CButtonComponent::CreateButtonComponent(
		GetInstance()->m_pCreditObj2, "", "", "T_CreditsStart_D.png", 256, 768, false, CREDITS_STATE, 3);
	GetInstance()->pScroller = CScrollerComponent::CreateScrollerComponent(
		GetInstance()->pButton, 0.0f, -65.0f, 1024.0f, 1024.0f);

	GetInstance()->pButton2 = CButtonComponent::CreateButtonComponent(
		GetInstance()->m_pCreditObj2, "", "", "T_CreditsEnd_D.png", 256, 1792, false, CREDITS_STATE, 3);
	GetInstance()->pScroller2 = CScrollerComponent::CreateScrollerComponent(
		GetInstance()->pButton2, 0.0f, -65.0f, 1024.0f, 1024.0f);
	GetInstance()->m_pCreditObj2->GetTransform()->ScaleFrame(2.0f, 1.0f, 1.0f);
}
void CCreditManager::Shutdown(void)
{
	//m_pCredits->SetIsActive(false);
	//m_pCredits->SetY(768);
	GetInstance()->pButton->SetScreenPosition(256,768);
	GetInstance()->pButton2->SetScreenPosition(256, 1792);
	GetInstance()->pScroller->SetY(768);
	GetInstance()->pScroller2->SetY(1792);

}
void CCreditManager::Update(void)
{
	//TSpriteData tSprite = GetInstance()->m_pBMFont->GetSpriteData();
	/*m_pCredits->SetWord(GetInstance()->m_szCredits);
	GetInstance()->fY += -50*GetInstance()->m_fDT;
	if(GetInstance()->fY <= -998)
	{
		GetInstance()->fY = 768;
	}
	m_pCredits->SetY((int)(GetInstance()->fY));
	m_pCredits->SetX((int)(GetInstance()->fX));
	m_pCredits->SetIsActive(true);
	m_pCredits->SetWord(GetInstance()->m_szCredits);*/
	//m_pCredits->DrawWord();
}
void CCreditManager::UpdateCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	TUpdateStateEvent* tEvent = (TUpdateStateEvent*)pEvent->GetData();
	float fDt = tEvent->m_fDeltaTime;
	GetInstance()->m_fDT = fDt;
	GetInstance()->m_fSeconds += fDt;
	GetInstance()->Update();
}



