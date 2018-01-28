#include "stdafx.h"
#include "CSyntheticMarketData.h"


CSyntheticMarketData::CSyntheticMarketData(const char* product, set<string> emits, CSyntheticCalculate* pCal)
{
	m_product = product;
	m_emits = emits;
	m_emit = nullptr;
	memset(m_emit_buf,0, sizeof(m_emit_buf));
	if (m_emits.size() == 1)
	{
		// ֻ�����һ��
		strncpy(m_emit_buf, emits.begin()->data(), sizeof(m_emit_buf));
		m_emit = m_emit_buf;
	}

	memset(&m_leg, 0, sizeof(m_leg));
	m_leg.pBuf = new char[LEG_DATA_BUF_LEN]();
	memset(m_leg.pBuf, 0, LEG_DATA_BUF_LEN);
	m_pCal = pCal;

	m_size = 0;
	m_count = 0;
	m_last_count = -1;
}


CSyntheticMarketData::~CSyntheticMarketData()
{
	if (m_leg.pBuf)
	{
		delete[] m_leg.pBuf;
		m_leg.pBuf = nullptr;
	}
}

void CSyntheticMarketData::Create(const char* instrument, double weight, char* pData)
{
	auto it = m_map.find(instrument);
	if (it == m_map.end())
	{
		LegData lmd = { 0 };
		lmd.weight = weight;
		lmd.pBuf = pData;
		m_map.insert(pair<string, LegData>(instrument, lmd));

		m_set.insert(instrument);
	}
	else
	{
	}
	// ���������������Register�е��ã��ϳɺ�Լ�ĳɷݴ�С�ǹ̶���
	m_size = m_map.size();
}

void CSyntheticMarketData::UpdateWeight()
{
	double sum = 0;
	for (auto iter = m_map.begin(); iter != m_map.end(); iter++)
	{
		sum += iter->second.weight;
	}
	for (auto iter = m_map.begin(); iter != m_map.end(); iter++)
	{
		iter->second.weight = iter->second.weight / sum;
	}
}

void* CSyntheticMarketData::Calculate(char* pBuf)
{
	if (m_pCal == nullptr)
	{
		return nullptr;
	}
	lock_guard<mutex> cl(m_cs);

	m_pCal->Begin(this, pBuf, &m_leg);

	for (auto iter = m_map.begin(); iter != m_map.end(); iter++)
	{
		// ���
		m_pCal->Sum(this, &iter->second, &m_leg);
	}

	for (auto iter = m_map.begin(); iter != m_map.end(); iter++)
	{
		// ��Ȩ��,��Ȩ���Ѿ����
		m_pCal->ForEach(this, &iter->second, &m_leg);
	}

	m_pCal->End(this, pBuf, &m_leg);

	return m_leg.pBuf + 1;
}

bool CSyntheticMarketData::CheckEmit(const char* instrument)
{
	// ��ͳ���ж�����������ʱ�������ڴ�����ʵ��ʼ��ʱ���Ѿ�����
	// ֻ��ǿ�м�һ����־λ
	if (m_size != m_count)
	{
		char ch = 0;
		for (auto iter = m_map.begin(); iter != m_map.end(); iter++)
		{
			auto c = iter->second.pBuf[0];
			ch += c;
		}
		m_count = ch;
	}
	if (m_last_count == m_count)
	{
		if (m_emit)
		{
			// ����strcmpҪ��set�е�findҪ�죬���ԡ�����
			return strcmp(m_emit, instrument) == 0;
		}
		// ��ͬ�ж��Ƿ�ָ����Լ
		auto it = m_emits.find(instrument);
		return it != m_emits.end();
	}
	else
	{
		m_last_count = m_count;
		// ��ͬ��ʾ�����𲽼������ݰ�
		return m_size == m_count;
	}
}

void* CSyntheticMarketData::GetValue()
{
	return m_leg.pBuf;
}

set<string> CSyntheticMarketData::GetInstruments()
{
	return m_set;
}