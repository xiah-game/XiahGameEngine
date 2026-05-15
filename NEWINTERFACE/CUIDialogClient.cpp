/****************************************************************************************************
	파 일 명:   CUIDialogClient.cpp
	만든날자:	2004/02/27  13:27
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUIDialogClient.h"

using namespace std;

namespace XiahGameEngine
{
	CUIDialogClient::CUIDialogClient() : m_pDlgMediator(NULL)
	{
	}

	CUIDialogClient::~CUIDialogClient()
	{
		delete m_pDlgMediator;
		m_pDlgMediator = NULL;
	}

	/**
	* 프레임 생성
	* \param pFrameData 프레임 정보
	* \return 성공여부
	*/
	bool CUIDialogClient::Create(sFrameData & pFrameData)
	{
		m_pDlgMediator = new CUIBasisDialog;

		if(!m_pDlgMediator)
			return false;

		return reinterpret_cast<CUIBasisDialog*>(m_pDlgMediator)->Create(pFrameData);  //((CUIBasisDialog*)m_pDlgMediator)->Create(pFrameData);
	}

	/**
	* 컨트롤 추가
	* \param &pCtrlData 추가될 정보
	* \return 성공여부
	*/
	bool CUIDialogClient::AddControl(sCtrlData &pCtrlData)
	{
		if(!m_pDlgMediator)
			return false;

		return reinterpret_cast<CUIBasisDialog*>(m_pDlgMediator)->AddControl(pCtrlData);  //((CUIBasisDialog*)m_pDlgMediator)->AddControl(pCtrlData);
	}

	/**
	* Draw
	*/
	void CUIDialogClient::Draw()
	{
		m_pDlgMediator->Draw();
	}

	/**
	* Set Show
	* \param nSubID 
	*/
	void CUIDialogClient::Show(const int nSubID)
	{
		m_pDlgMediator->Show(nSubID);
	}


	/**
	* Set Hide
	* \param nSubID 
	*/
	void CUIDialogClient::Hide(const int nSubID)
	{
		m_pDlgMediator->Hide(nSubID);
	}

	/**
	 * 갱신
	 * \param mouse 마우스 이벤트 정보
	 * \param nBefore 갱신 유형
	 */
	void CUIDialogClient::UpDate(sMouseEvent& mouse, int nBefore)
	{
		m_pDlgMediator->UpDate(mouse, nBefore);
	}

	void CUIDialogClient::ReCreate()
	{
		m_pDlgMediator->ReCreate();
	}

	/**
	 * 해제
	 */
	void CUIDialogClient::Destroy(void)
	{
		m_pDlgMediator->Destroy();
	}

	/**
	* 정보 변경
	* \param data 변경 내용
	*/
	void CUIDialogClient::DataChange(sChangeData& data)
	{
		m_pDlgMediator->DataChange(data);
	}

	/**
	 * Get Data
	 * \param data 가져올 정보
	 */
	void CUIDialogClient::GetData(sGetData& data)
	{
		m_pDlgMediator->GetData(data);
	}
};