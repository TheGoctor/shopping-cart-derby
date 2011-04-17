// FILE HEADER
#ifndef _CRENDERCOMP_H_
#define _CRENDERCOMP_H_

#include"..\\..\\IComponent.h"
#include"..\\..\\Managers\\Global Managers\\Rendering Managers\\DXRenderContext.h"

class DXRenderShape;
class CObject;

class CRenderComponent : public IComponent
{
	CObject* m_pcParent;
	DXRenderShape* m_cRenderShape;

public:
	CRenderComponent(CObject* pParent, int nModelID, DXRenderContext* pRC,
		void (*pfRenderFunc)(RenderNode&));

	~CRenderComponent(void);

	void Init(void);

	inline CObject* GetParent()
	{
		return m_pcParent;
	}

	inline DXRenderShape* GetRenderShape()
	{
		return m_cRenderShape;
	}
};

#endif // SCD_RENDERCOMP_H_