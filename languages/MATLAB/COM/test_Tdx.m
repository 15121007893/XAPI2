%% ����
td = Tdx_Connect();

% ��Ҫѭ���ȴ�����
SetWait(0);
Wait();

%% һ��Ҫ�ȴ���¼�ɹ�����
orders = ReqQryOrder(td)
positions = ReqQryInvestorPosition(td)
return;
