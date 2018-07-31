    <!DOCTYPE HTML>  
    <html>  
    <head>  
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">  
    <title>初七几点去香港</title>  
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
      <label for="other">其他时间</label>    
        <input type="text" name="other" id="other" size="60"  maxlength="128" />   
        <br />  
        <br />  
        <input type="submit" value="确定"  name="submit" />    
    </form>  
    </body>  
    </html>  
      
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
