%% ����
td = Tdx_Connect();

% ��Ҫѭ���ȴ�����
SetWait(0);
Wait();

%% һ��Ҫ�ȴ���¼�ɹ�����
orders = ReqQryOrder(td)
positions = ReqQryInvestorPosition(td)
return;


%% ͨ���б�ȡ����
w = WindConnect();
MarketData = GetQuoteFromWind(wind_code,w,1);
MarketData.stock_code = WindCode2StockCode(MarketData.wind_code);

% �����Ƿ���
MarketData.RT_OPEN_CHG = (MarketData.RT_OPEN - MarketData.RT_PRE_SETTLE)./MarketData.RT_PRE_SETTLE;

[m,n] = size(MarketData);
MarketData.price = zeros(m,1);

% ע�⣬���Ǹ��ݿ��̼����ü۸񣬶����Ǹ������¼�
for i=1:m
    RT_OPEN_CHG = MarketData.RT_OPEN_CHG(i);
    if RT_OPEN_CHG>0.07
        MarketData.price(i) = MarketData.RT_OPEN(i)*0.98;
    elseif RT_OPEN_CHG<-0.07
        MarketData.price(i) = MarketData.RT_OPEN(i)*0.99;
    else
        MarketData.price(i) = MarketData.RT_OPEN(i)*1;
        %MarketData.price(i) = MarketData.RT_OPEN(i) - MarketData.RT_PRE_SETTLE(i)*0.01;
    end
end

TradeList = MarketData(MarketData.Enable~=0,{'stock_code','price'});
[m,n] = size(TradeList);

TradeList.qty = ones(m,1).*500;

for i=1:m
    BuyLimit(td,TradeList.stock_code{i}, TradeList.qty(i), TradeList.price(i));
end

