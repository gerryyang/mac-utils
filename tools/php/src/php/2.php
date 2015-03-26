<?php 
//首先采用“fopen”函数打开文件，得到返回值的就是资源类型
$file_handle = fopen("/root/gerry_data/php/README2", "r");
if ($file_handle){
    //接着采用while循环一行行地读取文件，然后输出每行的文字
    while (!feof($file_handle)) { //判断是否到最后一行
        $line = fgets($file_handle); //读取一行文本
        echo $line; //输出一行文本
        echo "<br />"; //换行
    }
}
fclose($file_handle);//关闭文件
?>