<?php
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ���ڿͻ���C��UTF-8�鷳����������ط��ļ�Ҫ����ΪASCII������д���ļ�����ASCII��
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
function write_ini_file($assoc_arr, $path, $has_sections=FALSE) { 
    $content = ""; 
    if ($has_sections) { 
        foreach ($assoc_arr as $key=>$elem) { 
            $content .= "[".$key."]\n"; 
            foreach ($elem as $key2=>$elem2) { 
                if(is_array($elem2)) 
                { 
                    for($i=0;$i<count($elem2);$i++) 
                    { 
                        $content .= $key2."[] = \"".$elem2[$i]."\"\n"; 
                    } 
                } 
                else if($elem2=="") $content .= $key2." = \n"; 
                else $content .= $key2." = \"".$elem2."\"\n"; 
            } 
        } 
    } 
    else { 
        foreach ($assoc_arr as $key=>$elem) { 
            if(is_array($elem)) 
            { 
                for($i=0;$i<count($elem);$i++) 
                { 
                    $content .= $key2."[] = \"".$elem[$i]."\"\n"; 
                } 
            } 
            else if($elem=="") $content .= $key2." = \n"; 
            else $content .= $key2." = \"".$elem."\"\n"; 
        } 
    }
    if (!$handle = fopen($path, 'wt')) { 
        return false; 
    } 
    if (!fwrite($handle, $content)) { 
        return false; 
    } 
    fclose($handle); 
    return true; 
}

/*
    �Զ����ɾ������,����ɾ���ļ��͵ݹ�ɾ���ļ���
*/
function my_del_dir($path)
{
    if(is_dir($path))
    {
            $file_list= scandir($path);
            foreach ($file_list as $file)
            {
                if( $file!='.' && $file!='..')
                {
                    my_del_dir($path.'/'.$file);
                }
            }
            @rmdir($path);  //���ַ��������ж��ļ����Ƿ�Ϊ��,  ��Ϊ���ܿ�ʼʱ�ļ����Ƿ�Ϊ��,���������ʱ��,���ǿյ�     
    }
    else
    {
        @unlink($path);    //�������ط���û���Ҫ��@����һ��warning����,��������
    }
}

?>