<?php

  $time  = $_POST['time'];
  $other = $_POST['other'];

  if (isset($_POST['time']))
  {
    if ($other != "")
    {
      printf("%s <br />", $other);
    }

    printf("OK, 明天%s点我们在深圳湾口岸集合<br />", $time);
    printf("不见不散！<br />");
  }
  else if ($other != "")
  {
    printf("%s <br />", $other);
    printf("让我考虑一下，等下答复你！<br />");
  }
  else if (isset($_POST['other']))
  {
    printf("你还没有回答我呢！<br />");
  }
  
?>