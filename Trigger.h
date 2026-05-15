#pragma once

namespace XiahGameEngine
{
	// Delegate클래스
	class CTrigger
	{
	public:
		virtual int Invoke() = 0;
	public:
		CTrigger(){};
		virtual ~CTrigger(){};

		unsigned long	m_nParam;
	};

	typedef int (*TRIGGER_EVENT_HANDLER)(unsigned long);

	// Static또는 Global한 함수핸들러용 트리거
	class CStaticTrigger : public CTrigger
	{
	public:
		int Invoke()
		{          
			return m_pHandler( m_nParam);
		}

		CStaticTrigger(TRIGGER_EVENT_HANDLER pfn,unsigned long nParam)
			:m_pHandler(pfn)
		{
			m_nParam = nParam;
		}
		virtual ~CStaticTrigger(){};

	private:
		TRIGGER_EVENT_HANDLER m_pHandler;
		//unsigned long	m_nParam;
	};

	// 클래스의 멤버 변수를 트리거로 사용할 경우
	template <typename T>
	class  CClassTrigger : public CTrigger
	{
	public:
		int Invoke()
		{
			return (*m_pClass.*m_pHandler)( m_nParam);
		}

		CClassTrigger(T* t,int (T::*pfn)(unsigned long),unsigned long nParam)
			: m_pClass(t)
			, m_pHandler(pfn)
		{
			
		}
		virtual ~CClassTrigger(){}

	private:
		T*	m_pClass;
		int (T::*m_pHandler)(unsigned long);
		//unsigned long	m_nParam;
	};

	// TriggerSet
	class CTriggerList : public std::vector<CTrigger*>
	{
	public:
		XIAHGE_API CTriggerList();
		XIAHGE_API virtual ~CTriggerList();

		XIAHGE_API BOOL SetSize(int nCount);
		XIAHGE_API BOOL SetTrigger(int nIndex,CTrigger *pT);
		XIAHGE_API int Invoke(int nIndex);
		XIAHGE_API int Invoke(int nIndex,unsigned long param);
	};
};