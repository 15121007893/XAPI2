#include "stdafx.h"
#include "CSyntheticCalculate_DepthMarketDataNField.h"

#include "ApiStruct.h"


void CSyntheticCalculate_DepthMarketDataNField_OpenInterest::Begin(CSyntheticMarketData* pCall, char* pBuf, LegData* pOut)
{
	auto cI = pBuf;
	auto cO = pOut->pBuf;
	auto pI = (DepthMarketDataNField*)(pBuf+1);
	auto pO = (DepthMarketDataNField*)(pOut->pBuf+1);

	memset(pO, 0, sizeof(DepthMarketDataNField));
	memset(pOut, 0, sizeof(LegData));

	pOut->pBuf = cO;
}

void CSyntheticCalculate_DepthMarketDataNField_OpenInterest::Sum(CSyntheticMarketData* pCall, LegData * pIn, LegData * pOut)
{
	auto cI = pIn->pBuf;
	auto cO = pOut->pBuf;
	auto pI = (DepthMarketDataNField*)(pIn->pBuf + 1);
	auto pO = (DepthMarketDataNField*)(pOut->pBuf + 1);

	pO->OpenInterest += pI->OpenInterest;
	pO->Volume += pI->Volume;
	// ����������������һ��sum
	pOut->sum += pI->OpenInterest;
}

void CSyntheticCalculate_DepthMarketDataNField_OpenInterest::ForEach(CSyntheticMarketData* pCall, LegData * pIn, LegData * pOut)
{
	auto cI = pIn->pBuf;
	auto cO = pOut->pBuf;
	auto pI = (DepthMarketDataNField*)(pIn->pBuf + 1);
	auto pO = (DepthMarketDataNField*)(pOut->pBuf + 1);
	// ����Ȩ�أ�����ָ��Ȩ�ص����ݲ���Ҫ�޸ģ���ָ���ĳɷݹ�
	pIn->weight = pI->OpenInterest / pOut->sum;
	// �����¼۸�
	pO->LastPrice += pI->LastPrice*pIn->weight;
}

void CSyntheticCalculate_DepthMarketDataNField_OpenInterest::End(CSyntheticMarketData* pCall, char* pBuf, LegData* pOut)
{
	auto cI = pBuf;
	auto cO = pOut->pBuf;
	auto pI = (DepthMarketDataNField*)(pBuf + 1);
	auto pO = (DepthMarketDataNField*)(pOut->pBuf + 1);

	pO->Size = sizeof(DepthMarketDataNField);
	pO->BidCount = 0;
	pO->ActionDay = pI->ActionDay;
	pO->TradingDay = pI->TradingDay;
	pO->UpdateTime = pI->UpdateTime;
	pO->UpdateMillisec = pI->UpdateMillisec;
	strcpy(pO->ExchangeID, pI->ExchangeID);

	strcpy(pO->InstrumentID, pCall->GetProduct().c_str());
	strcpy(pO->Symbol, pO->InstrumentID);
}

void CSyntheticCalculate_DepthMarketDataNField_Weight::Begin(CSyntheticMarketData* pCall, char* pBuf, LegData* pOut)
{
	auto cI = pBuf;
	auto cO = pOut->pBuf;
	auto pI = (DepthMarketDataNField*)(pBuf + 1);
	auto pO = (DepthMarketDataNField*)(pOut->pBuf + 1);

	memset(pO, 0, sizeof(DepthMarketDataNField));
	memset(pOut, 0, sizeof(LegData));

	pOut->pBuf = cO;
}

void CSyntheticCalculate_DepthMarketDataNField_Weight::Sum(CSyntheticMarketData* pCall, LegData * pIn, LegData * pOut)
{
	auto cI = pIn->pBuf;
	auto cO = pOut->pBuf;
	auto pI = (DepthMarketDataNField*)(pIn->pBuf + 1);
	auto pO = (DepthMarketDataNField*)(pOut->pBuf + 1);

	// ������Ӧ��Ȩ�غ�Ϊ1��100
	pOut->weight += pIn->weight;
}

void CSyntheticCalculate_DepthMarketDataNField_Weight::ForEach(CSyntheticMarketData* pCall, LegData * pIn, LegData * pOut)
{
	auto cI = pIn->pBuf;
	auto cO = pOut->pBuf;
	auto pI = (DepthMarketDataNField*)(pIn->pBuf + 1);
	auto pO = (DepthMarketDataNField*)(pOut->pBuf + 1);
	// �����¼۸������Ȩ���ڼ���ʱ��������
	pO->LastPrice += pI->LastPrice*pIn->weight;
}

void CSyntheticCalculate_DepthMarketDataNField_Weight::End(CSyntheticMarketData* pCall, char* pBuf, LegData* pOut)
{
	auto cI = pBuf;
	auto cO = pOut->pBuf;
	auto pI = (DepthMarketDataNField*)(pBuf + 1);
	auto pO = (DepthMarketDataNField*)(pOut->pBuf + 1);

	pO->Size = sizeof(DepthMarketDataNField);
	pO->BidCount = 0;
	pO->ActionDay = pI->ActionDay;
	pO->TradingDay = pI->TradingDay;
	pO->UpdateTime = pI->UpdateTime;
	pO->UpdateMillisec = pI->UpdateMillisec;
	strcpy(pO->ExchangeID, pI->ExchangeID);

	strcpy(pO->InstrumentID, pCall->GetProduct().c_str());
	strcpy(pO->Symbol, pO->InstrumentID);
}

CSyntheticCalculate* CSyntheticCalculateFactory_XAPI::Create(string method)
{
	if (method == "OpenInterest")
	{
		return new CSyntheticCalculate_DepthMarketDataNField_OpenInterest();
	}
	else if (method == "Weight")
	{
		return new CSyntheticCalculate_DepthMarketDataNField_Weight();
	}
	return nullptr;
}