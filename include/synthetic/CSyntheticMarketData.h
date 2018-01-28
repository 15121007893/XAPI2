#pragma once

#include <list>
#include <map>
#include <mutex>
#include <set>

using namespace std;

#define LEG_DATA_BUF_LEN (512)
// �����ڲ������õĽṹ��
struct LegData
{
	// Ȩ�أ��������ⲿ�Ѿ�ָ���ã�Ҳ��������ͣ�Ȼ������Ȩ��
	double weight;
	// ��Ӵ˱��������ڽ�����ܳ��ֵ������������
	double sum;
	// ����
	int count;
	// ʹ���ڴ�������ݲ�ͬ����,�������ĵ�һ���ֽڷ�0��ʾ�����Ѿ�׼����
	char* pBuf;
};

class CSyntheticMarketData;

class CSyntheticCalculate
{
	// ע�⣺pBuf��LegData->pBuf����0��λ�Ǳ��λ
public:
	virtual void Begin(CSyntheticMarketData* pCall, char* pBuf, LegData* pOut) = 0;
	virtual void Sum(CSyntheticMarketData* pCall, LegData* pIn, LegData* pOut) = 0;
	virtual void ForEach(CSyntheticMarketData* pCall, LegData* pIn, LegData* pOut) = 0;
	virtual void End(CSyntheticMarketData* pCall, char* pBuf, LegData* pOut) = 0;
};

class CSyntheticCalculateFactory
{
public:
	virtual CSyntheticCalculate* Create(string method) = 0;
};

// �������ĳһ���ϳ�ָ��
// ��IF000,�ϻ���Ʒָ����
// ������key��������Լ��
class CSyntheticMarketData
{
public:
	CSyntheticMarketData(const char* product, set<string> emit, CSyntheticCalculate* pCal);
	~CSyntheticMarketData();

	void Create(const char* instrument, double weight, char* pData);
	void UpdateWeight();
	// ע�⣬���ڼ�����ڴ�飬��0��λ�Ǳ��λ
	void* Calculate(char* pBuf);
	void* GetValue();

	set<string> GetEmits() { return m_emits; }
	string GetProduct() { return m_product; }
	set<string> GetInstruments();

	bool CheckEmit(const char* instrument);

private:
	string							m_product;
	set<string>						m_emits;
	const char*						m_emit;
	char							m_emit_buf[128];
	map<string, LegData>	m_map;
	LegData							m_leg;
	mutex							m_cs;
	CSyntheticCalculate*			m_pCal;
	set<string>						m_set;
	int								m_size;
	int								m_count;
	int								m_last_count;
};

