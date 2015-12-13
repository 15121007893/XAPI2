#include "stdafx.h"
#include "SingleUser.h"

#include "../QuantBox_Queue/MsgQueue.h"
#include "TypeConvert.h"
#include "TraderApi.h"

#define QUERY_TIME_MIN	(3)
#define QUERY_TIME_MAX	(60)

int Today(int day)
{
	time_t now = time(0);
	now += day * 86400;
	struct tm *ptmNow = localtime(&now);

	return (ptmNow->tm_year + 1900) * 10000
		+ (ptmNow->tm_mon + 1) * 100
		+ ptmNow->tm_mday;
}

void CreateID(char* pOut, char* pDate, char*pZh, char* wtbh)
{
	if (pDate == nullptr || strlen(pDate) == 0)
	{
		sprintf(pOut, "%d:%s:%s", Today(0), pZh, wtbh);
	}
	else
	{
		sprintf(pOut, "%s:%s:%s", pDate, pZh, wtbh);
	}
}

CSingleUser::CSingleUser(CTraderApi* pApi)
{
	m_pApi = pApi;
	m_pClient = nullptr;
	memset(m_UserID, 0, sizeof(m_UserID));
}


CSingleUser::~CSingleUser()
{
}

void CSingleUser::OutputQueryTime(time_t t, double db, const char* szSource)
{
	LogField* pField = (LogField*)m_msgQueue->new_block(sizeof(LogField));

	sprintf(pField->Message, "UserID:%s,Source:%s,Add:%d,Time:%s", m_UserID, szSource, (int)db, ctime(&t));

	// ȥ�����Ļس�
	int len = strlen(pField->Message);
	pField->Message[len - 1] = 0;

	m_msgQueue->Input_NoCopy(ResponeType::OnLog, m_msgQueue, m_pClass, true, 0, pField, sizeof(LogField), nullptr, 0, nullptr, 0);
}

void CSingleUser::CheckThenQueryOrder(time_t _now)
{
	if (_now > m_QueryOrderTime)
	{
		double _queryTime = QUERY_TIME_MAX;
		m_QueryOrderTime = time(nullptr) + _queryTime;
		OutputQueryTime(m_QueryOrderTime, _queryTime, "QueryOrder");

		ReqQryOrder();
	}
}

void CSingleUser::CheckThenQueryTrade(time_t _now)
{
	if (_now > m_QueryTradeTime)
	{
		double _queryTime = QUERY_TIME_MAX;
		m_QueryOrderTime = time(nullptr) + _queryTime;
		OutputQueryTime(m_QueryOrderTime, _queryTime, "QueryOrder");

		ReqQryOrder();
	}
}

void CSingleUser::ReqQryOrder()
{

}


void CSingleUser::ReqQryTrade()
{

}

int CSingleUser::OnRespone_ReqQryOrder(CTdxApi* pApi, RequestRespone_STRUCT* pRespone)
{
	WTLB_STRUCT** ppRS = nullptr;
	CharTable2WTLB(pRespone->ppFieldInfo, pRespone->ppResults, &ppRS, pRespone->Client);

	// ����ǰ��գ���˵֮ǰ�Ѿ���չ�һ����
	m_NewOrderList.clear();

	// ��δ��ɵģ����Ϊtrue
	bool IsDone = true;
	// ��δ�걨�ģ����Ϊtrue
	bool IsNotSent = false;
	// �и��µ�
	bool IsUpdated = false;

	if (ppRS)
	{
		int i = 0;
		while (ppRS[i])
		{
			// ������ί�й���
			if (ppRS[i]->MMBZ_ != MMBZ_Cancel)
			{
				// ��Ҫ�������뵽һ���ط����ڼ��㣬�������ʱ�ģ���Ҫɾ��
				OrderField* pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));

				WTLB_2_OrderField_0(ppRS[i], pField);
				CreateID(pField->ID, ppRS[i]->WTRQ, ppRS[i]->GDDM, ppRS[i]->WTBH);

				m_NewOrderList.push_back(pField);

				if (!ZTSM_IsDone(ppRS[i]->ZTSM_))
				{
					IsDone = false;
				}
				if (ZTSM_IsNotSent(ppRS[i]->ZTSM_))
				{
					IsNotSent = true;
				}

				// ��Ҫ���䱣����������ֻ����һ�Σ�����ÿ�ζ������أ�������Ϊֻ����һ�μ��ɣ�����������������
				unordered_map<string, WTLB_STRUCT*>::iterator it = m_pApi->m_id_api_order.find(pField->ID);
				if (it == m_pApi->m_id_api_order.end())
				{
					WTLB_STRUCT* pWTField = (WTLB_STRUCT*)m_msgQueue->new_block(sizeof(WTLB_STRUCT));
					memcpy(pWTField, ppRS[i], sizeof(WTLB_STRUCT));
					m_pApi->m_id_api_order.insert(pair<string, WTLB_STRUCT*>(pField->ID, pWTField));
				}
			}
			++i;
		}
	}

	// ί���б�
	// 1.�����Ķ���Ҫ���
	// 2.�ϵĿ��Ƿ��б仯
	++m_OrderNotUpdateCount;

	int i = 0;
	list<OrderField*>::iterator it2 = m_OldOrderList.begin();
	for (list<OrderField*>::iterator it = m_NewOrderList.begin(); it != m_NewOrderList.end(); ++it)
	{
		OrderField* pField = *it;

		bool bUpdate = false;
		if (i >= m_OldOrderList.size())
		{
			bUpdate = true;
		}
		else
		{
			// ��ͬλ�õĲ���
			OrderField* pOldField = *it2;
			if (pOldField->LeavesQty != pField->LeavesQty || pOldField->Status != pField->Status)
			{
				bUpdate = true;
			}
		}

		if (bUpdate)
		{
			IsUpdated = true;
			m_OrderNotUpdateCount = 0;

			// ������ҵ��µ�ʱ��ί�У����޸ĺ󷢳���
			unordered_map<string, OrderField*>::iterator it = m_pApi->m_id_platform_order.find(pField->ID);
			if (it == m_pApi->m_id_platform_order.end())
			{
				// ��Ϊ�ϴ����ɵĿ����ں���ɾ�ˣ�����Ҫ����һ��
				OrderField* pField_ = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));
				memcpy(pField_, pField, sizeof(OrderField));

				m_pApi->m_id_platform_order.insert(pair<string, OrderField*>(pField_->ID, pField_));
			}
			else
			{
				OrderField* pField_ = it->second;
				memcpy(pField_, pField, sizeof(OrderField));
			}

			m_msgQueue->Input_Copy(ResponeType::OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
		}

		// ǰһ������Ϊ�գ��ƶ�����һ��ʱ��Ҫע��
		if (it2 != m_OldOrderList.end())
		{
			++it2;
		}

		++i;
	}

	// ��������������ֹ�ڴ�й©
	for (list<OrderField*>::iterator it = m_OldOrderList.begin(); it != m_OldOrderList.end(); ++it)
	{
		OrderField* pField = *it;
		m_msgQueue->delete_block(pField);
	}

	// ������
	m_OldOrderList.clear();
	m_OldOrderList = m_NewOrderList;
	m_NewOrderList.clear();

	double _queryTime = 0;
	if (!IsDone)
	{
		if (!IsUpdated)
		{
			// û�и��£��Ƿ�Ҫ�����
			_queryTime = 0.5 * QUERY_TIME_MAX + QUERY_TIME_MIN;
		}

		// ��û����ɵģ���Ҫ��ʱ��ѯ
		if (IsNotSent)
		{
			// ��û�걨�ģ��Ƿ�û�ڽ���ʱ�䣿�����
			_queryTime = 0.5 * QUERY_TIME_MAX + QUERY_TIME_MIN;
		}
		else
		{
			// �����ǽ���ʱ���ˣ��Ƿ���Ҫ����
			_queryTime = 2 * QUERY_TIME_MIN;
			// ������Щ�ҵ�һ�춼����ɽ���������һֱ���²�̫�࣬��һ�²�ѯ����
			if (m_OrderNotUpdateCount >= 3)
			{
				_queryTime = 0.5 * QUERY_TIME_MAX + QUERY_TIME_MIN;
			}
		}
	}
	else
	{
		// ȫ����ˣ����Բ��������
		_queryTime = 5 * QUERY_TIME_MAX;
	}

	m_QueryOrderTime = time(nullptr) + _queryTime;
	OutputQueryTime(m_QueryOrderTime, _queryTime, "NextQueryOrder_QueryOrder");

	// �����ɽ���ѯ���
	if (IsUpdated)
	{
		// ί�п����ǳ�����Ҳ�п����ǳɽ��ˣ��Ͻ���һ��
		_queryTime = 0;
		m_QueryTradeTime = time(nullptr) + _queryTime;
		OutputQueryTime(m_QueryTradeTime, _queryTime, "NextQueryTrade_QueryOrder");
	}
	else
	{
		// ί��û�б仯���ǳɽ���û�б�Ҫ����ô����
		_queryTime = 5 * QUERY_TIME_MAX;
		m_QueryTradeTime = time(nullptr) + _queryTime;
		OutputQueryTime(m_QueryTradeTime, _queryTime, "NextQueryTrade_QueryOrder");
	}

	return 0;
}


double GetTradeListQty(list<TradeField*> &tradeList, int count)
{
	double Qty = 0;
	int i = 0;
	for (list<TradeField*>::iterator it = tradeList.begin(); it != tradeList.end(); ++it)
	{
		++i;
		if (i > count)
		{
			break;
		}

		TradeField* pField = *it;
		Qty += pField->Qty;
	}
	return Qty;
}

void TradeList2TradeMap(list<TradeField*> &tradeList, unordered_map<string, TradeField*> &tradeMap)
{
	// ֻ�����������new��deleteӦ��û������
	for (unordered_map<string, TradeField*>::iterator it = tradeMap.begin(); it != tradeMap.end(); ++it)
	{
		TradeField* pNewField = it->second;
		delete[] pNewField;
	}
	tradeMap.clear();

	// �������Լƴ�ӳ�
	for (list<TradeField*>::iterator it = tradeList.begin(); it != tradeList.end(); ++it)
	{
		TradeField* pField = *it;
		unordered_map<string, TradeField*>::iterator it2 = tradeMap.find(pField->ID);
		if (it2 == tradeMap.end())
		{
			TradeField* pNewField = new TradeField;
			memcpy(pNewField, pField, sizeof(TradeField));
			tradeMap[pField->ID] = pNewField;
		}
		else
		{
			TradeField* pNewField = it2->second;
			pNewField->Price = pField->Price;
			pNewField->Qty += pField->Qty;
		}
	}
}

int CSingleUser::OnRespone_ReqQryTrade(CTdxApi* pApi, RequestRespone_STRUCT* pRespone)
{
	CJLB_STRUCT** ppRS = nullptr;
	CharTable2CJLB(pRespone->ppFieldInfo, pRespone->ppResults, &ppRS, pRespone->Client);

	// ����ǰ��գ���˵֮ǰ�Ѿ���չ�һ����
	m_NewTradeList.clear();

	if (ppRS)
	{
		// ���óɽ���ŵĴ�С���ж�������
		if (!m_TradeListReverse)
		{
			int count = GetCountStructs((void**)ppRS);
			if (count > 1)
			{
				// ת�����ֵıȽϣ��Ƿ���з����ֵ�������֣�
				long CJBH0 = atol(ppRS[0]->CJBH);
				long CJBH1 = atol(ppRS[count - 1]->CJBH);
				if (CJBH0 > CJBH1)
				{
					m_TradeListReverse = true;
				}
			}
		}

		int i = 0;
		while (ppRS[i])
		{
			TradeField* pField = (TradeField*)m_msgQueue->new_block(sizeof(TradeField));

			CJLB_2_TradeField(ppRS[i], pField);

			if (m_TradeListReverse)
			{
				// ��̩������ı�����ɵ��ŵ�һ������Ҫ����һ��
				m_NewTradeList.push_front(pField);
			}
			else
			{
				m_NewTradeList.push_back(pField);
			}

			++i;
		}
	}

	// �²�����ķ������ˣ���̩�кϲ��ɽ��������������δ���
	// ��ͬID����Ҫ�ۼӣ��з����ۼӲ���Ӧ�ģ�Ӧ������
	// ͬ�������ģ�Ҳ�п������е��б仯����δ���
	bool bTryMerge = false;
	int OldTradeListCount = m_OldTradeList.size();
	int NewTradeListCount = m_NewTradeList.size();

	if (NewTradeListCount < OldTradeListCount)
	{
		// ���������ˣ�Ӧ���Ǻϲ���
		bTryMerge = true;
	}
	else if (OldTradeListCount == 0)
	{
		// �����һ�ε�Ϊ�գ�������β�������Ǻϲ�����û�кϲ�����û�й�ϵ������û�ϲ�������
	}
	else if (NewTradeListCount == OldTradeListCount)
	{
		// �������䣬���п��������е�һ�����ֳɽ��ĸ��£����Լ��һ��

		double OldQty = GetTradeListQty(m_OldTradeList, m_OldTradeList.size());
		double NewQty = GetTradeListQty(m_NewTradeList, m_NewTradeList.size());
		if (NewQty != OldQty)
		{
			// ͬ�����ȳɽ��������˱仯�������Ǻϲ����б�����һ���³ɽ���
			bTryMerge = true;
		}
	}
	else
	{
		// ��������ˣ�ֻҪ�����ϴεĲ����б仯����Ҫ���һ��
		double OldQty = GetTradeListQty(m_OldTradeList, m_OldTradeList.size());
		double NewQty = GetTradeListQty(m_NewTradeList, m_NewTradeList.size());
		if (NewQty != OldQty)
		{
			bTryMerge = true;
		}
	}

	if (bTryMerge)
	{
		// �ϲ��б�Ĵ�����
		// ����ϴ��Ǻϲ�����ξ�û�б�Ҫ������һ����
		if (m_OldTradeMap.size() == 0 || !m_LastIsMerge)
		{
			for (unordered_map<string, TradeField*>::iterator it = m_OldTradeMap.begin(); it != m_OldTradeMap.end(); ++it)
			{
				TradeField* pField = it->second;
				delete[] pField;
			}
			m_OldTradeMap.clear();

			TradeList2TradeMap(m_OldTradeList, m_OldTradeMap);
		}
		TradeList2TradeMap(m_NewTradeList, m_NewTradeMap);
		CompareTradeMapAndEmit(m_OldTradeMap, m_NewTradeMap);

		// ����
		for (unordered_map<string, TradeField*>::iterator it = m_OldTradeMap.begin(); it != m_OldTradeMap.end(); ++it)
		{
			TradeField* pField = it->second;
			delete[] pField;
		}
		m_OldTradeMap.clear();
		m_OldTradeMap = m_NewTradeMap;
		m_NewTradeMap.clear();
		m_LastIsMerge = true;
	}
	else
	{
		// ��ͨ�Ĵ�����
		CompareTradeListAndEmit(m_OldTradeList, m_NewTradeList);
		m_LastIsMerge = false;
	}

	// ��������������ֹ�ڴ�й©
	for (list<TradeField*>::iterator it = m_OldTradeList.begin(); it != m_OldTradeList.end(); ++it)
	{
		TradeField* pField = *it;
		m_msgQueue->delete_block(pField);
	}

	// ������
	m_OldTradeList.clear();
	m_OldTradeList = m_NewTradeList;
	m_NewTradeList.clear();

	return 0;
}

void CSingleUser::CompareTradeMapAndEmit(unordered_map<string, TradeField*> &oldMap, unordered_map<string, TradeField*> &newMap)
{
	for (unordered_map<string, TradeField*>::iterator it = newMap.begin(); it != newMap.end(); ++it)
	{
		TradeField* pNewField = it->second;
		unordered_map<string, TradeField*>::iterator it2 = oldMap.find(pNewField->ID);
		if (it2 == oldMap.end())
		{
			// û�ҵ�,���µ�
			m_msgQueue->Input_Copy(ResponeType::OnRtnTrade, m_msgQueue, m_pClass, 0, 0, pNewField, sizeof(TradeField), nullptr, 0, nullptr, 0);
		}
		else
		{
			TradeField* pOldField = it2->second;
			int Qty = pNewField->Qty - pOldField->Qty;
			if (Qty > 0)
			{
				// �б仯�ĵ�
				TradeField* pField = new TradeField;
				memcpy(pField, pNewField, sizeof(TradeField));
				pField->Qty = Qty;
				m_msgQueue->Input_Copy(ResponeType::OnRtnTrade, m_msgQueue, m_pClass, 0, 0, pNewField, sizeof(TradeField), nullptr, 0, nullptr, 0);
				delete[] pField;
			}
		}
	}
}

void CSingleUser::CompareTradeListAndEmit(list<TradeField*> &oldList, list<TradeField*> &newList)
{
	int i = 0;
	list<TradeField*>::iterator it2 = oldList.begin();
	for (list<TradeField*>::iterator it = newList.begin(); it != newList.end(); ++it)
	{
		TradeField* pField = *it;

		bool bUpdate = false;
		if (i >= oldList.size())
		{
			bUpdate = true;
		}
		//else
		//{
		//	// ��ͬλ�õĲ���
		//	TradeField* pOldField = *it2;
		//	if (pOldField->Qty != pField->Qty)
		//	{
		//		bUpdate = true;
		//	}
		//}

		if (bUpdate)
		{
			m_msgQueue->Input_Copy(ResponeType::OnRtnTrade, m_msgQueue, m_pClass, 0, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);
		}

		// ǰһ������Ϊ�գ��ƶ�����һ��ʱ��Ҫע��
		if (it2 != oldList.end())
		{
			++it2;
		}

		++i;
	}
}



int CSingleUser::OnRespone_ReqUserLogin(CTdxApi* pApi, RequestRespone_STRUCT* pRespone)
{
	// ��ѯ�ɶ��б���̩֤ȯ����һ��ʼ��᷵�ط�֪����[1122]
	GDLB_STRUCT** ppRS = nullptr;
	CharTable2Login(pRespone->ppResults, &ppRS, pRespone->Client);

	int count = GetCountStructs((void**)ppRS);

	if (count > 0)
	{
		for (int i = 0; i < count; ++i)
		{
			InvestorField* pField = (InvestorField*)m_msgQueue->new_block(sizeof(InvestorField));

			GDLB_2_InvestorField(ppRS[i], pField);

			m_msgQueue->Input_NoCopy(ResponeType::OnRspQryInvestor, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(InvestorField), nullptr, 0, nullptr, 0);
		}
	}

	return 0;
}

int CSingleUser::OnRespone_ReqQryInvestor(CTdxApi* pApi, RequestRespone_STRUCT* pRespone)
{
	GDLB_STRUCT** ppRS = nullptr;
	CharTable2GDLB(pRespone->ppFieldInfo, pRespone->ppResults, &ppRS, pRespone->Client);

	int count = GetCountStructs((void**)ppRS);

	for (int i = 0; i < count; ++i)
	{
		InvestorField* pField = (InvestorField*)m_msgQueue->new_block(sizeof(InvestorField));

		GDLB_2_InvestorField(ppRS[i], pField);

		m_msgQueue->Input_NoCopy(ResponeType::OnRspQryInvestor, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(InvestorField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CSingleUser::OnRespone_ReqQryTradingAccount(CTdxApi* pApi, RequestRespone_STRUCT* pRespone)
{
	ZJYE_STRUCT** ppRS = nullptr;
	CharTable2ZJYE(pRespone->ppFieldInfo, pRespone->ppResults, &ppRS, pRespone->Client);

	int count = GetCountStructs((void**)ppRS);
	for (int i = 0; i < count; ++i)
	{
		AccountField* pField = (AccountField*)m_msgQueue->new_block(sizeof(AccountField));

		ZJYE_2_AccountField(ppRS[i], pField);

		//// �����ʽ��˺Ų鲻�������ֹ�����
		//if (strlen(pField->AccountID) <= 0)
		//{
		//	// ���˻���������
		//	strcpy(pField->AccountID, m_pApi->GetAccount());
		//}

		m_msgQueue->Input_NoCopy(ResponeType::OnRspQryTradingAccount, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(AccountField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CSingleUser::OnRespone_ReqQryInvestorPosition(CTdxApi* pApi, RequestRespone_STRUCT* pRespone)
{
	GFLB_STRUCT** ppRS = nullptr;
	CharTable2GFLB(pRespone->ppFieldInfo, pRespone->ppResults, &ppRS, pRespone->Client);

	int count = GetCountStructs((void**)ppRS);
	for (int i = 0; i < count; ++i)
	{
		PositionField* pField = (PositionField*)m_msgQueue->new_block(sizeof(PositionField));

		// Ӧ������һ�£�����һ���˺Ŷ�Ӧ���ж�����������˻�
		GFLB_2_PositionField(ppRS[i], pField);

		m_msgQueue->Input_NoCopy(ResponeType::OnRspQryInvestorPosition, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(PositionField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CSingleUser::OnRespone_ReqOrderInsert(CTdxApi* pApi, RequestRespone_STRUCT* pRespone)
{
	pRespone->ppResults;
	pRespone->pErr;

	return 0;
}

int CSingleUser::OnRespone_ReqOrderAction(CTdxApi* pApi, RequestRespone_STRUCT* pRespone)
{
	return 0;
}