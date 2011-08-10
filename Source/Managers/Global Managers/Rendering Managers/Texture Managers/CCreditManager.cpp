////////////////////////////////////////////////////////////////////////////////
//  File			:	"CIntroManager.cpp"
//
//  Author			:	Huy Nguyen (HN)
//
//  Date Created	:	05/18/11
//
//	Last Changed	:	07/24/11
//
//	Changed By		:	HN			
//
//  Purpose			:	Wrapper class for managing the Credit screen
////////////////////////////////////////////////////////////////////////////////
#include "CCreditManager.h"
#include "..\..\Event Manager\CEventManager.h"
#include "..\..\Event Manager\CIDGen.h"
#include "..\..\Event Manager\EventStructs.h"
#include "..\..\..\..\CObject.h"
#include "CBitmapFont.h"
#include "CTextureManager.h"
#include "..\..\Object Manager\CObjectManager.h"
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

	GetInstance()->m_pCreditObj2 = CObjectManager::GetInstance()->CreateObject("CreditObj");
	//make the receipt paper
	//start sheet
	GetInstance()->pButton = CButtonComponent::CreateButtonComponent(
		GetInstance()->m_pCreditObj2, "", "", "T_CreditsStart_D.png", 256, 768, false, CREDITS_STATE, 3);
	GetInstance()->pScroller = CScrollerComponent::CreateScrollerComponent(
		GetInstance()->pButton, 0.0f, -65.0f, 1024.0f, 1024.0f);
	//mid sheet
	GetInstance()->pButton2 = CButtonComponent::CreateButtonComponent(
		GetInstance()->m_pCreditObj2, "", "", "T_CreditsMid_D.png", 256, 1792, false, CREDITS_STATE, 3);
	GetInstance()->pScroller2 = CScrollerComponent::CreateScrollerComponent(
		GetInstance()->pButton2, 0.0f, -65.0f, 1024.0f, 1024.0f);
	//end sheet
	GetInstance()->pButton3 = CButtonComponent::CreateButtonComponent(
		GetInstance()->m_pCreditObj2, "", "", "T_CreditsEnd_D.png", 256, 2802, false, CREDITS_STATE, 3);
	GetInstance()->pScroller3 = CScrollerComponent::CreateScrollerComponent(
		GetInstance()->pButton3, 0.0f, -65.0f, 1024.0f, 1024.0f);

	GetInstance()->m_pCreditObj2->GetTransform()->ScaleFrame(2.0f, 1.0f, 1.0f);
	
}
void CCreditManager::InitCredits(IEvent* /*pEvent*/, IComponent* /*pComp*/)
{
//	GetInstance()->m_pCreditObj2 = CObjectManager::GetInstance()->CreateObject("CreditObj");
		
//	GetInstance()->pButton->SetScreenPosition(256,768);
//	GetInstance()->pButton2->SetScreenPosition(256, 1792);
//	GetInstance()->pScroller->SetY(768);
//	GetInstance()->pScroller2->SetY(1792);
	GetInstance()->pButton->SetIsActive(true);
	GetInstance()->pScroller->SetIsActive(true);
	GetInstance()->pButton2->SetIsActive(true);
	GetInstance()->pScroller2->SetIsActive(true);
	GetInstance()->pButton3->SetIsActive(true);
	GetInstance()->pScroller3->SetIsActive(true);
	
	//make the receipt paper
//	GetInstance()->pButton = CButtonComponent::CreateButtonComponent(
//		GetInstance()->m_pCreditObj2, "", "", "T_CreditsStart_D.png", 256, 768, false, CREDITS_STATE, 3);
//	GetInstance()->pScroller = CScrollerComponent::CreateScrollerComponent(
//		GetInstance()->pButton, 0.0f, -65.0f, 1024.0f, 1024.0f);
//
//	GetInstance()->pButton2 = CButtonComponent::CreateButtonComponent(
//		GetInstance()->m_pCreditObj2, "", "", "T_CreditsEnd_D.png", 256, 1792, false, CREDITS_STATE, 3);
//	GetInstance()->pScroller2 = CScrollerComponent::CreateScrollerComponent(
//		GetInstance()->pButton2, 0.0f, -65.0f, 1024.0f, 1024.0f);
//	GetInstance()->m_pCreditObj2->GetTransform()->ScaleFrame(2.0f, 1.0f, 1.0f);

}
void CCreditManager::Shutdown(void)
{
	//m_pCredits->SetIsActive(false);
	//m_pCredits->SetY(768);
	GetInstance()->pButton->SetIsActive(false);
	GetInstance()->pScroller->SetIsActive(false);
	GetInstance()->pButton2->SetIsActive(false);
	GetInstance()->pScroller2->SetIsActive(false);
	GetInstance()->pButton3->SetIsActive(false);
	GetInstance()->pScroller3->SetIsActive(false);
	GetInstance()->pButton->SetScreenPosition(256,768);
	GetInstance()->pButton2->SetScreenPosition(256, 1792);
	GetInstance()->pButton3->SetScreenPosition(256, 2762);
	GetInstance()->pScroller->SetY(768);
	GetInstance()->pScroller2->SetY(1792);
	GetInstance()->pScroller3->SetY(2802);

}
void CCreditManager::Update(void)
{
}
void CCreditManager::UpdateCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	TUpdateStateEvent* tEvent = (TUpdateStateEvent*)pEvent->GetData();
	float fDt = tEvent->m_fDeltaTime;
	GetInstance()->m_fDT = fDt;
	GetInstance()->m_fSeconds += fDt;
	GetInstance()->Update();
}



