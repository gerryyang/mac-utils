<!DOCTYPE HTML>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>活动统计 v1.0 leftright: gerryyang</title>
</head>

<body>
<form action="<?php echo $_SERVER['PHP_SELF']; ?>" method="post">
  <label for="7am">早上7点到深圳湾口岸集合</label>
  <input type="radio" name="time" id="7am" value="7" />
  <br />
  <label for="8am">早上8点到深圳湾口岸集合</label>
  <input type="radio" name="time" id="8am" value="8" />
  <br />
  <label for="9am">早上9点到深圳湾口岸集合</label>
  <input type="radio" name="time" id="9am" value="9" />
  <br />
  <label for="other">其他</label>  
    <input type="text" name="other" id="other" size="30"  maxlength="29" /> 
    <br />
    <br />
    <input type="submit" value="确定"  name="submit" />  
    <input type="submit" value="查询"  name="query" />  
</form>
</body>
</html>

<?php

  $time  = $_POST['time'];
  $other = $_POST['other'];
  $query = $_POST['query'];

  if (isset($_POST['time']))
  {
    if ($other != "")
    {
      printf("%s <br />", $other);
    }

    $info = sprintf("OK, 明天%s点我们在深圳湾口岸集合, 不见不散！", $time);
    printf("%s <br />", $info);

    do_save($info);


  }
  else if ($other != "")
  {
    $info = sprintf("%s", $other);
    printf("%s <br />", $info);

    do_save($info);
  }
  else if ($query == "查询")
  {
    do_query();
  }
  else if (isset($_POST['other']))
  {
    printf("你还没有回答我呢！<br />");
  }

  function connect_db(&$link)
  {
    $link = mysql_connect('localhost', 'test', '123456');
    if (!$link) {
      die('Could not connect: ' . mysql_error());
    }
    echo 'Connected successfully <br \>'; 
  
    $db = "research";
    if (!mysql_select_db($db, $link)) 
    {
      die('Could not select database ' . $db);
    }
  }

  function do_save($info)
  {
    connect_db($link);

    $time = date('Y-m-d H:i:s');
    $query = 'insert into whentohk(time, info) values(\'' . $time . '\', \'' . $info . '\')';
    printf("%s <br />", $query);
    $result = mysql_query($query, $link);
    if (!$result) {
      $message  = 'Invalid query: ' . mysql_error() . "\n";
      $message .= 'Whole query: ' . $query;
      die($message);
    }

    mysql_free_result($result);
  }

  function do_query()
  {
    connect_db($link);

    $query = 'select time, info from whentohk order by time desc';
    printf("%s <br />", $query);
    $result = mysql_query($query, $link);
    if (!$result) {
      $message  = 'Invalid query: ' . mysql_error() . "\n";
      $message .= 'Whole query: ' . $query;
      die($message);
    }

    echo "<hr />";
    printf("统计结果如下：<br />");
    while ($row = mysql_fetch_assoc($result)) 
    {
      printf("%s&nbsp;&nbsp;&nbsp;&nbsp;%s <br />", $row['time'], $row['info']);
    }

    mysql_free_result($result);
  }

?>
