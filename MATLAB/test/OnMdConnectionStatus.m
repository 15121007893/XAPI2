function OnMdConnectionStatus(sender,arg)

import QuantBox.XAPI.*;

global md;

disp('MD')
% �������ӻر�
disp(arg.status);

switch arg.status
    case QuantBox.ConnectionStatus.Disconnected
        % ��ӡ������Ϣ
        disp(Extensions_GBK.Text(arg.userLogin));
    case QuantBox.ConnectionStatus.Done
        % �������飬֧��";"�ָ�
        md.Subscribe('IF1602;IF1603;IF1606','');
end

end
