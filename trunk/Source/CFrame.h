/*************************************************************************
* Filename:			SCD_Frame.h
* Mod. Date:		03/31/2011
* Mod. Initials:	RN
* Author(s):		Raymond W. Nieves, Huy Nguyen
* Purpose:			Allows for the creation of matrix/transform hierarchies
*************************************************************************/
#ifndef _CFRAME_H_
#define _CFRAME_H_

#include <list>
#include "Managers\Global Managers\Rendering Managers\dxutil.h"
#include "Managers\Global Managers\Memory Manager\CAllocator.h"

class CObject;

class CFrame
{
private:

	D3DXVECTOR3 m_lastPosition;//last world position
	D3DXMATRIX m_mLocalMatrix;
	D3DXMATRIX m_mWorldMatrix;
	CFrame *m_pParent;
	std::list<CFrame*, CAllocator<CFrame*>> m_vpChildren;
	unsigned int m_uFlags;

public:
	enum {DIRTY = 1};

	CFrame(void) : m_uFlags(0), m_pParent(NULL) 
	{
		// Set to Orginal
		D3DXMatrixIdentity(&m_mLocalMatrix);
		D3DXMatrixIdentity(&m_mWorldMatrix);
		m_lastPosition = D3DXVECTOR3(0,0,0);
	};

	virtual ~CFrame(void){};

	CFrame *GetParentFrame(void)
	{
		return m_pParent;
	}
	void SetParent(CFrame *pParent)
	{
		m_pParent = pParent;
	}

	void AddChildFrame(CFrame *pChild)
	{
		if(pChild->m_pParent != NULL)
		{
			if(pChild->m_pParent != this)
			{
				pChild->m_pParent->RemoveChildFrame(pChild);
				m_vpChildren.push_back(pChild);
				pChild->m_pParent = this;
			}
		}
		else
		{
			m_vpChildren.push_back(pChild);
			pChild->m_pParent = this;
		}
	}

	CFrame *RemoveChildFrame(CFrame *pRChild)
	{
		CFrame *pTempFrame = NULL;
		std::list<CFrame*, CAllocator<CFrame*>>::iterator iter;
		for(iter = m_vpChildren.begin(); iter != m_vpChildren.end(); ++iter)
		{
			if(pRChild == (*iter))
			{
				pTempFrame = (*iter);
				m_vpChildren.erase(iter);
				pTempFrame->m_pParent = NULL;
				break;
			}
		}
		return pTempFrame;
		//		m_vpChildren.remove(pRChild);
	}
	//sets all the children of a frame to "DIRTY"
	void SetChildFlags(void)
	{
		std::list<CFrame*, CAllocator<CFrame*>>::iterator iter;
		for(iter = m_vpChildren.begin(); iter != m_vpChildren.end(); ++iter)
		{
			(*iter)->SetChildFlags();
		}
	}

	void Update(void)
	{
		m_lastPosition = D3DXVECTOR3(GetWorldMatrix()._41, 
			GetWorldMatrix()._42 ,GetWorldMatrix()._43);
		if(m_uFlags != DIRTY)
		{
			m_uFlags = DIRTY;
			std::list<CFrame*, CAllocator<CFrame*>>::iterator iter;
			for(iter = m_vpChildren.begin(); iter != m_vpChildren.end(); ++iter)
			{
				(*iter)->Update();
			}
		}
	}

	D3DXMATRIX& GetLocalMatrix(void)
	{
		return m_mLocalMatrix;
	}

	const D3DXMATRIX& GetWorldMatrix(void)
	{
		if(m_uFlags == DIRTY)
		{
			if(m_pParent != NULL)
			{
				D3DXMatrixMultiply(&m_mWorldMatrix, &m_mLocalMatrix, &m_pParent->GetWorldMatrix());
				return m_mWorldMatrix;
			}
			else
			{
				m_mWorldMatrix = m_mLocalMatrix;
				return m_mWorldMatrix;
			}
		}
		else
		{
			return m_mWorldMatrix;
		}
	}
	void RotateFrame(D3DXVECTOR3 vRotAxis, float fAngle)
	{
		D3DXMATRIX rotMat;
		D3DXMatrixRotationAxis(&rotMat, &vRotAxis, fAngle);
		D3DXMatrixMultiply(&m_mLocalMatrix, &rotMat, &m_mLocalMatrix);
		Update();
	}
	void ScaleFrame(float fScaleX, float fScaleY, float fScaleZ)
	{
		D3DXMATRIX scaleMat;
		D3DXMatrixScaling(&scaleMat, fScaleX, fScaleY, fScaleZ);
		D3DXMatrixMultiply(&m_mLocalMatrix, &m_mLocalMatrix, &scaleMat);
		Update();
	}
	D3DXVECTOR3 GetLastWorldPosition()
	{
		return m_lastPosition;
	}
	D3DXVECTOR3 GetWorldPosition()
	{
		return D3DXVECTOR3(GetWorldMatrix()._41, 
			GetWorldMatrix()._42 ,GetWorldMatrix()._43);
	}
	D3DXVECTOR3 GetLocalPosition()
	{
		return D3DXVECTOR3(GetLocalMatrix()._41, 
			GetLocalMatrix()._42 ,GetLocalMatrix()._43);
	}
	void TranslateFrame(D3DXVECTOR3 vTrans)
	{
		m_lastPosition.x = GetWorldMatrix()._41;
		m_lastPosition.y = GetWorldMatrix()._42;
		m_lastPosition.z = GetWorldMatrix()._43;
		D3DXMATRIX translateMat;
		D3DXMatrixTranslation(&translateMat, vTrans.x, vTrans.y, vTrans.z);
		D3DXMatrixMultiply(&m_mLocalMatrix, &m_mLocalMatrix, &translateMat);
		Update();
	}

protected:
	static D3DXMATRIX QuatToRotMat(D3DXVECTOR4 tQuats)
	{
		float xx, xy, xz, xw,
			yy, yz, yw,
			zz, zw;
		xx = tQuats.x * tQuats.x;
		xy = tQuats.x * tQuats.y;
		xz = tQuats.x * tQuats.z;
		xw = tQuats.x * tQuats.w;

		yy = tQuats.y * tQuats.y;
		yz = tQuats.y * tQuats.z;
		yw = tQuats.y * tQuats.w;

		zz = tQuats.z * tQuats.z;
		zw = tQuats.z * tQuats.w;

		D3DXMATRIX tRots;
		D3DXMatrixIdentity(&tRots);
		tRots[0] = 1 - 2 * (yy + zz);
		tRots[1] =     2 * (xy - zw);
		tRots[2] =     2 * (xz + yw);
		tRots[4] =     2 * (xy + zw);
		tRots[5] = 1 - 2 * (xx + zz);
		tRots[6] =     2 * (yz - xw);
		tRots[8] =     2 * (xz - yw);
		tRots[9] =     2 * (yz + xw);
		tRots[10]= 1 - 2 * (xx + yy);
		tRots[3] = tRots[7] = tRots[11] = tRots[12] = tRots[13] = tRots[14] = 0.0f;
		tRots[15] = 1;
		return tRots;
	}
};

#endif	// _CFRAME_H_