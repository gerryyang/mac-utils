

http://104.131.173.242/index.php


SSH Quick Install

```
wget --content-disposition http://www.mybb.com/download/latest -O mybb.zip
unzip mybb.zip "Upload/*"
mv Upload/* .
rm -Rf Upload mybb.zip
mv inc/config.default.php inc/config.php
chmod -R 0777 cache uploads inc/settings.php inc/config.php
```

mysql -umybbuser -pmybbuser

```
mysql> show databases;
+--------------------+
| Database           |
+--------------------+
| information_schema |
| mybb               |
+--------------------+
2 rows in set (0.00 sec)

mysql> use mybb
Database changed
mysql> show tables;
Empty set (0.01 sec)

mysql> show tables;
+--------------------------+
| Tables_in_mybb           |
+--------------------------+
| mybb_adminlog            |
| mybb_adminoptions        |
| mybb_adminsessions       |
| mybb_adminviews          |
| mybb_announcements       |
| mybb_attachments         |
| mybb_attachtypes         |
| mybb_awaitingactivation  |
| mybb_badwords            |
| mybb_banfilters          |
| mybb_banned              |
| mybb_buddyrequests       |
| mybb_calendarpermissions |
| mybb_calendars           |
| mybb_captcha             |
| mybb_datacache           |
| mybb_delayedmoderation   |
| mybb_events              |
| mybb_forumpermissions    |
| mybb_forums              |
| mybb_forumsread          |
| mybb_forumsubscriptions  |
| mybb_groupleaders        |
| mybb_helpdocs            |
| mybb_helpsections        |
| mybb_icons               |
| mybb_joinrequests        |
| mybb_mailerrors          |
| mybb_maillogs            |
| mybb_mailqueue           |
| mybb_massemails          |
| mybb_moderatorlog        |
| mybb_moderators          |
| mybb_modtools            |
| mybb_mycode              |
| mybb_polls               |
| mybb_pollvotes           |
| mybb_posts               |
| mybb_privatemessages     |
| mybb_profilefields       |
| mybb_promotionlogs       |
| mybb_promotions          |
| mybb_questions           |
| mybb_questionsessions    |
| mybb_reportedcontent     |
| mybb_reputation          |
| mybb_searchlog           |
| mybb_sessions            |
| mybb_settinggroups       |
| mybb_settings            |
| mybb_smilies             |
| mybb_spamlog             |
| mybb_spiders             |
| mybb_stats               |
| mybb_tasklog             |
| mybb_tasks               |
| mybb_templategroups      |
| mybb_templates           |
| mybb_templatesets        |
| mybb_themes              |
| mybb_themestylesheets    |
| mybb_threadprefixes      |
| mybb_threadratings       |
| mybb_threads             |
| mybb_threadsread         |
| mybb_threadsubscriptions |
| mybb_threadviews         |
| mybb_userfields          |
| mybb_usergroups          |
| mybb_users               |
| mybb_usertitles          |
| mybb_warninglevels       |
| mybb_warnings            |
| mybb_warningtypes        |
+--------------------------+
74 rows in set (0.00 sec)
```

# Refer

https://www.digitalocean.com/community/tutorials/a-straightforward-guide-on-installing-web-facing-applications-on-a-vps

https://www.digitalocean.com/community/tutorials/how-to-launch-your-site-on-a-new-ubuntu-14-04-server-with-lamp-sftp-and-dns

https://www.digitalocean.com/community/tutorials/how-to-set-up-a-host-name-with-digitalocean

http://docs.mybb.com/1.8/install/

