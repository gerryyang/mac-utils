<?php
/**
 * wechat php
 * update: 
 * 20170118: add weekly walk record
 * before: only response a text
 */

//define your token
define("TOKEN", "2147483648");
$wechatObj = new wechatCallbackapiTest();
// use once when verify developer interface
//$wechatObj->valid();
$wechatObj->responseMsg();

class wechatCallbackapiTest
{
	public function valid()
	{
		$echoStr = $_GET["echostr"];

		//valid signature , option
		if($this->checkSignature()){
			echo $echoStr;
			exit;
		}
	}

	public function responseMsg()
	{
		//get post data, May be due to the different environments
		$postStr = $GLOBALS["HTTP_RAW_POST_DATA"];

		//extract post data
		if (!empty($postStr)){

			$postObj = simplexml_load_string($postStr, 'SimpleXMLElement', LIBXML_NOCDATA);
			$fromUsername = $postObj->FromUserName; // 发送方帐号（一个OpenID）
			$toUsername = $postObj->ToUserName;     // 开发者微信号 
			$keyword = trim($postObj->Content);
			$time = time();
			$textTpl = "<xml>
				<ToUserName><![CDATA[%s]]></ToUserName>
				<FromUserName><![CDATA[%s]]></FromUserName>
				<CreateTime>%s</CreateTime>
				<MsgType><![CDATA[%s]]></MsgType>
				<Content><![CDATA[%s]]></Content>
				<FuncFlag>0</FuncFlag>
				</xml>";             
			if(!empty( $keyword ))
			{
				// request format: type a b c ...
				$type = strstr($keyword, " ", true);
				if($type == "r" || $type == "R") {
					// req format: r/R name steps 
					$data = ltrim(strstr($keyword, " "), " ");
					$name = strstr($data, " ", true);
					$steps = ltrim(strstr($data, " "), " ");
					if(empty($name) || empty($steps)) {
						$contentStr = "请回复: r/R name steps";
					} else {
						do_save($name, $steps);
						$contentStr = "上报成功";
					}
					$msgType = "text";
					$resultStr = sprintf($textTpl, $fromUsername, $toUsername, $time, $msgType, $contentStr);
					echo $resultStr;

				} else if($keyword == "q" || $keyword == "Q") {

					$msgType = "text";
					$contentStr = do_query();
					$resultStr = sprintf($textTpl, $fromUsername, $toUsername, $time, $msgType, $contentStr);
					echo $resultStr;

				} else {

					// 返回一个文本消息
					$msgType = "text";
					$contentStr = "欢迎关注gerryyang，我会分享一些人文和技术内容。您可以回复m，查看关于此订阅号的信息，或者通过点击右上角图标 - 查看历史消息，或者查看我的Web存档：http://blog.csdn.net/delphiwcdj (目前使用开发者接口)";
					$resultStr = sprintf($textTpl, $fromUsername, $toUsername, $time, $msgType, $contentStr);
					echo $resultStr;
				}

			}else{
				echo "Input something...";
			}

		}else {
			echo "gerry: say hello";
			exit;
		}
	}

	private function checkSignature()
	{
		$signature = $_GET["signature"];
		$timestamp = $_GET["timestamp"];
		$nonce = $_GET["nonce"];	

		$token = TOKEN;
		$tmpArr = array($token, $timestamp, $nonce);
		sort($tmpArr, SORT_STRING);
		$tmpStr = implode( $tmpArr );
		$tmpStr = sha1( $tmpStr );

		if( $tmpStr == $signature ){
			return true;
		}else{
			return false;
		}
	}
}

function connect_db(&$link)  
{  
	$link = mysql_connect('127.0.0.1', 'test', 'test');  
	if (!$link) {  
		die('Could not connect: ' . mysql_error());  
	}  
	echo 'Connected successfully <br \>';   

	$db = "sport_conf";  
	if (!mysql_select_db($db, $link))   
	{  
		die('Could not select database ' . $db);  
	}  
}  

function do_save($name, $steps)  
{  
	connect_db($link);  

	$time = date('Y-m-d H:i:s');  
	$query = 'insert into weekly_walk(time, name, info) values(\'' . $time . '\', \'' . $name . '\', \'' . $steps . '\')';  
	printf("%s <br />", $query);  
	$result = mysql_query($query, $link);  
	if (!$result) {  
		$message  = 'Invalid query: ' . mysql_error() . "\n";  
		$message .= 'Whole query: ' . $query;  
		die($message);  
	}  
}  

function do_query()  
{  
	connect_db($link);  

	$query = 'select time, name, info from weekly_walk order by time desc';  
	printf("%s <br />", $query);  

	$result = mysql_query($query, $link);  
	if (!$result) {  
		$message  = 'Invalid query: ' . mysql_error() . "\n";  
		$message .= 'Whole query: ' . $query;  
		die($message);  
	}  

	echo "<hr />";  
	printf("结果：<br />");  
	$ans = "结果：";
	while ($row = mysql_fetch_assoc($result))   
	{  
		$ans = $ans . " " . $row['time'] . " " . $row['name'] . " " . $row['info'];
		printf("%s  %s  %s <br />", $row['time'], $row['name'], $row['info']);  
	}  

	mysql_free_result($result);  

	return $ans;
}  

?>
