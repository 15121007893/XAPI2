#ifndef _QUEUE_ENUM_H_
#define _QUEUE_ENUM_H_

/// ��������
enum RequestType :char
{
	GetApiType = 0,
	GetApiVersion,
	GetApiName,

	Create, // ����
	Release, // ����
	Register, // ע����ն��лص�
	Config,		// ���ò���

	Connect, // ��ʼ/����
	Disconnect, // ֹͣ/�Ͽ�

	Clear, // ����
	Process, // ����

	Subscribe,	// ����
	Unsubscribe, // ȡ������

	SubscribeQuote, // ����ѯ��
	UnsubscribeQuote, // ȡ������ѯ��

	ReqOrderInsert,
	ReqQuoteInsert,
	ReqOrderAction,
	ReqQuoteAction,
};

///��ѯ
enum QueryType :char
{
	ReqQryInstrument = 32,
	ReqQryTradingAccount,
	ReqQryInvestorPosition,

	ReqQryOrder,
	ReqQryTrade,
	ReqQryQuote,

	ReqQryInstrumentCommissionRate,
	ReqQryInstrumentMarginRate,
	ReqQrySettlementInfo,
	ReqQryInvestor,

	ReqQryHistoricalTicks,
	ReqQryHistoricalBars,
};

///��Ӧ
enum ResponeType :char
{
	OnConnectionStatus = 64,
	OnRtnError,
	OnLog,

	OnRtnDepthMarketData,
	OnRspQryInstrument,
	OnRspQryTradingAccount,
	OnRspQryInvestorPosition,
	OnRspQrySettlementInfo,

	OnRspQryOrder,
	OnRspQryTrade,
	OnRspQryQuote,

	OnRtnOrder,
	OnRtnTrade,
	OnRtnQuote,

	OnRtnQuoteRequest,

	OnRspQryHistoricalTicks,
	OnRspQryHistoricalBars,
	OnRspQryInvestor,

	OnFilterSubscribe,
};

// >=100��ʾAPI�Զ���

#endif
