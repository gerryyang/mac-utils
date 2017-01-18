<?php
/**
 * wechat php test
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
				// 返回一个文本消息
				$msgType = "text";
				//$contentStr = "Welcome to wechat world!";
				$contentStr = "欢迎关注gerryyang，我会分享一些人文和技术内容。您可以回复m，查看关于此订阅号的信息，或者通过点击右上角图标 - 查看历史消息，或者查看我的Web存档：http://blog.csdn.net/delphiwcdj (目前使用开发者接口)";
				$resultStr = sprintf($textTpl, $fromUsername, $toUsername, $time, $msgType, $contentStr);
				echo $resultStr;
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

?>
