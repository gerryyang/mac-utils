---
layout: post
title:  "Graylog in Action"
date:   2024-01-16 20:22:00 +0800
categories: 云原生
---

* Do not remove this line (it will not be displayed)
{:toc}


# What Is Graylog?

> [Graylog](https://en.wikipedia.org/wiki/Graylog) - A centralized Log Management System (LMS)

**Graylog, Inc** is a log management and security analytics software company based in Houston, Texas. Their main product is a log management software which is also called **Graylog** (styled as **graylog**).

These days we deal with an abundance of data. This data comes from various sources like devices, applications, and operating systems. A centralized Log Management System (LMS) like Graylog provides a means to aggregate, organize, and make sense of all this data.

Log files are essentially text files. They contain an abundance of information--application name, IP address, time stamp, and source destination. All applications and even operating systems themselves create these logs containing massive amounts of data, which needs to be parsed if we want to make any sense of it.

An LMS must also be efficient in collecting and parsing petabytes of data. Once it has been parsed, log data can provide extremely useful information for forensic investigations, threat hunting, and business analytics in general. Whatever the use case, Graylog can help businesses look further into their data and save time and human resources.


# [Installing Graylog](https://go2docs.graylog.org/5-2/downloading_and_installing_graylog/installing_graylog.html?tocpath=Downloading%20and%20Installing%20Graylog%7CInstalling%20Graylog%7C_____0)

> **Warning**:
>
> 1. We caution you not to install or upgrade Elasticsearch to 7.11 or later! It is not supported. Doing so will break your instance!
>
> 2. Graylog 5.2 is the last version to include support for Elasticsearch (7.10.2 only)! We recommend you use OpenSearch 2.x as your data node to support Graylog 5.2.


## Compatibility Matrix

![graylog_compatibility](/assets/images/202401/graylog_compatibility.png)

* https://github.com/KongZ/charts/tree/main/charts/graylog
* https://go2docs.graylog.org/5-2/downloading_and_installing_graylog/installing_graylog_operations.html


# Graylog Architecture

Graylog 的架构专为日志管理和分析而设计，使其成为一种高效的日志管理系统（LMS）。该架构旨在处理时间轴上的文本数据和指标数据，确保日志数据的高效准确管理。Graylog 的架构由几个组件组成，每个组件负责日志管理的不同方面，包括处理、索引和数据访问。

1. 数据摄取和转发器：Graylog 使用数据摄取工具或转发器从各种来源收集日志数据。这些转发器可以通过 REST API 进行管理和配置，使系统管理员能够轻松地从远程主机选择日志文件以供摄取到系统中。
2. 数据处理器：数据处理器负责解析和处理日志数据。它们对数据进行规范化和丰富，使数据更有价值且易于分析。
3. 索引器：索引器负责存储和索引日志数据。它们确保数据有组织且易于搜索。
4. 数据访问：Graylog 为用户提供 Web 界面，以访问和分析日志数据。用户可以根据时间范围执行搜索，查看所有转发器、数据处理器和索引器的运行状况和配置，并监控整个系统的运行状况。
5. 可伸缩性：Graylog 的架构允许水平伸缩，使其能够适应不断变化的用例和不断增长的数据量。系统的不同部分具有不同的硬件需求，可以根据需要独立进行伸缩。
6. 集中式管理：Graylog 的架构确保所有服务紧密集成，允许整个系统的高效管理和配置。管理员可以在一个中心位置监控所有组件的运行状况和配置，确保不会错过任何重要数据。

总之，Graylog 的架构专为日志管理而设计，为处理日志数据提供了高效且可伸缩的解决方案。其组件协同工作以处理、索引和让日志数据可供分析，确保用户可以有效地管理和监控他们的日志数据。


> Hint: Please note that Graylog cannot modify saved log data. It provides the core centralized log management functionality you need to aggregate, organize, and interpret data so that you can derive meaningful insights while maintaining data integrity.


# Graylog Core Features

Graylog 作为一种灵活的工具，具有许多增强其实用性的功能。

1. 流（[Streams](https://go2docs.graylog.org/5-0/making_sense_of_your_log_data/streams.html)）作为传入消息的一种标签形式。流实时将消息路由到各个类别，团队规则指导 Graylog 将消息路由到适当的流。
2. 流用于将数据路由到索引以进行存储。它们还用于控制对数据的访问，并将消息路由到解析、丰富和其他修改。然后，流确定要存档哪些消息。
3. Graylog [搜索页面](https://go2docs.graylog.org/5-0/making_sense_of_your_log_data/how_to_search_your_log_data.html?Highlight=search%20)是用于直接搜索日志的界面。Graylog 使用简化的语法，非常类似于 Lucene。相对或绝对时间范围可以从下拉菜单中配置。可以保存搜索或将其可视化为可直接从搜索屏幕中添加到仪表板的仪表板小部件。
4. 用户可以配置自己的视图，并可以选择查看事件消息的摘要或完整数据。
5. Graylog [仪表板](https://go2docs.graylog.org/5-0/interacting_with_your_log_data/dashboards.html)是日志事件中包含的信息的可视化或汇总。每个仪表板由一个或多个小部件填充。小部件使用从字段值（如计数、平均值或总计）派生的数据可视化或汇总事件日志数据。用户可以创建指示器、图表、图形和地图来可视化数据。
6. 仪表板小部件和仪表板布局是可配置的。Graylog 的基于角色的访问控制仪表板访问。用户可以通过内容包导入和导出仪表板。
7. 通过使用由条件组成的事件定义创建[警报](https://go2docs.graylog.org/5-0/interacting_with_your_log_data/alerts_and_notifications.html)。满足给定条件时，它将作为事件存储并可用于触发通知。
8. [内容包](https://go2docs.graylog.org/5-0/what_more_can_graylog_do_for_me/content_packs.html?Highlight=content%20packs)加速了针对特定数据源的设置过程。内容包可以包括输入/提取器、流、仪表板、警报和管道处理器。例如，用户可以创建自定义输入、流、仪表板和警报以支持安全用例。然后，用户可以导出内容包并将其导入到新安装的 Graylog 实例上，以节省配置时间和精力。
9. 用户可以通过 Graylog 市场下载其他用户创建、共享和支持的内容包。
10. [索引](https://go2docs.graylog.org/5-0/setting_up_graylog/index_model.html)是 OpenSearch 和 Elasticsearch 中数据存储的基本单元。索引集为存储数据的保留、分片和复制提供配置。诸如保留和轮换策略之类的值是按索引设置的，因此不同的数据可能受到不同的处理规则。
11. Graylog [Sidecar](https://go2docs.graylog.org/5-0/getting_in_log_data/graylog_sidecar.html) 是一个代理，用于管理日志发货程序的机群，如 Beats 或 NXLog。这些日志发货程序用于从 Linux 和 Windows 服务器收集操作系统日志。日志发货程序读取本地写入的平面文件日志，然后将其发送到集中式日志管理解决方案。Graylog 支持作为后端管理任何日志发货程序。
12. Graylog 的[处理管道](https://go2docs.graylog.org/5-0/making_sense_of_your_log_data/pipelines.html)使用户能够针对特定类型的事件运行一个规则或一系列规则。与流绑定，管道允许在消息流经 Graylog 时进行路由、拒绝列表、修改和丰富。


# The Graylog Extended Log Format (GELF)

Graylog also introduces the Graylog Extended Log Format (GELF), which improves on the limitations of standard syslog, such as:

* Limited to length of 1024 bytes. Inadequate space for payloads like backtraces.

* No data types in structured syslog. Numbers and strings are indistinguishable.

* The RFCs are strict enough, but there are too many syslog dialects to parse them all.

* No compression.

Improvements on these issues make GELF a better choice for logging from within applications. The Graylog Marketplace offers libraries and appenders for easily implementing GELF in many programming languages and logging frameworks. Also, GELF can be sent via UDP so it cannot break your application from within your logging class.

For more information please refer to [Parsing Log Files in Graylog](https://www.graylog.org/post/parsing-log-files-in-graylog-overview?_gl=1*yr0gka*_ga*MTkxNTQ1MDA4Ni4xNzA1Mzc0NzE0*_ga_NCQ7VMMZNQ*MTcwNjY4MTAzNi43LjEuMTcwNjY4MjQyMy4wLjAuMA..).


# The Basis: Graylog + Data Node + MongoDB

Log data is stored in a data node, either Elasticsearch or OpenSearch. Both are open source search engines. Your data node allows you to index and search through all the messages in your Graylog message database.

This combination grants users the ability to conduct an in depth analysis of petabytes of data. Because your data is accessible to your data node you are not dependent on a third party search tool. This also means that you have great flexibility in customizing your own search queries. You also have access to the many open-source projects and tools provided by Elasticsearch or OpenSearch.

The [Graylog Server](https://go2docs.graylog.org/5-0/what_is_graylog/what_is_graylog.htm) component sits in the middle and works around your data node which is a full text search engine and not a log management system. It also builds an abstraction layer on top of it to make data access as easy as possible without having to select indices and write tedious time range selection filters, etc. Just submit the search query and Graylog will take care of the rest for you.

Graylog uses MongoDB to store data. Only metadata such as user information and stream configurations are stored here, not log data. So MongoDB will not have a huge system impact. It runs alongside the Graylog server processes and takes up minimal space.

# Community and Resources

[The Graylog Community](https://community.graylog.org/?_gl=1*1wvi38d*_ga*MTkxNTQ1MDA4Ni4xNzA1Mzc0NzE0*_ga_NCQ7VMMZNQ*MTcwNjY4MTAzNi43LjEuMTcwNjY4MjQyMy4wLjAuMA..) is 8,000 members strong. These Graylog users make valuable contributions to the product by asking questions, offering input for better performance, and providing support for fellow community members.

[The Graylog Marketplace](https://community.graylog.org/c/marketplace/31?_gl=1*1wvi38d*_ga*MTkxNTQ1MDA4Ni4xNzA1Mzc0NzE0*_ga_NCQ7VMMZNQ*MTcwNjY4MTAzNi43LjEuMTcwNjY4MjQyMy4wLjAuMA..) is where you can find plugins, extensions, content packs, a GELF library, and other solutions contributed by developers and community members.


# [Planning Your Deployment](https://go2docs.graylog.org/5-2/planning_your_deployment/planning_your_deployment.html?tocpath=Planning%20Your%20Deployment%7C_____0)

There are a few rules of thumb when scaling resources for Graylog:

* Graylog nodes should have a focus on CPU power. These also serve the user interface to the browser.

* Elasticsearch/OpenSearch nodes should have as much RAM as possible and the fastest disks you can get. Everything depends on I/O speed here.

* MongoDB is storing meta information and configuration data and doesn’t need many resources.

> Also keep in mind that ingested messages are only stored in Elasticsearch. If you have data loss in the Elasticsearch cluster, the messages are gone - except if you have created backups of the indices.


## Minimum Setup

This is a minimum Graylog setup that can be used for smaller, non-critical, or test setups. None of the components are redundant, and they are easy and quick to setup.

![graylog_small_setup](/assets/images/202401/graylog_small_setup.png)



## Bigger Production Setup

This is a setup for bigger production environments. It has several Graylog nodes behind a load balancer distributing the processing load.
The load balancer can ping the Graylog nodes via on the Graylog REST API to check if they are alive and take dead nodes out of the cluster.

![graylog_bigger_setup](/assets/images/202401/graylog_bigger_setup.png)

How to plan and configure such a setup is covered in our [Multi-node Setup guide](https://go2docs.graylog.org/5-0/setting_up_graylog/multi-node_setup.html).
Some guides on the [Graylog Marketplace](https://marketplace.graylog.org/?_gl=1*d9xh9l*_ga*MTkxNTQ1MDA4Ni4xNzA1Mzc0NzE0*_ga_NCQ7VMMZNQ*MTcwNjY4MTAzNi43LjEuMTcwNjY4MzMxMS4wLjAuMA..) also offer some ideas how you can use RabbitMQ (AMQP) or Apache Kafka to add some queuing to your setup.


# [Planning Your Log Collection](https://go2docs.graylog.org/5-2/planning_your_deployment/planning_your_log_collection.htm?tocpath=Planning%20Your%20Deployment%7C_____1)


## Collection Methods

A decision must be made as to how the logs will be collected. After a list of event sources have been determined, the next step is to decide the method of collection for each source. It is critical to understand what method each event source uses and what resources may be required.

For example, if a log shipper will be required to read logs from a local file on all servers, the log shipper must be selected and tested before deployment. In other cases, proprietary APIs or software tools must be employed and integrated. In some cases, changes to the event sources themselves (security devices, network hardware, or applications) may be required. Additional planning is often required to deploy and maintain these collection methods over time.

Graylog supports many input types out of the box and many more are available in [Graylog Marketplace](https://marketplace.graylog.org/?_gl=1*vku9n8*_ga*MTkxNTQ1MDA4Ni4xNzA1Mzc0NzE0*_ga_NCQ7VMMZNQ*MTcwNjY4MTAzNi43LjEuMTcwNjY4MzkzOS4wLjAuMA..). A list of input types that Graylog supports can be found in the [Getting Started Guide](https://go2docs.graylog.org/5-2/setting_up_graylog/initial_configuration_settings.html).

A successful, well planned collection of messages is the first step in enabling the user to gain the most benefit from Graylog. For step by step instruction on how to get your logs into Graylog, [please see this video](https://www.graylog.org/videos/getting-your-logs-into-graylog?_gl=1*vku9n8*_ga*MTkxNTQ1MDA4Ni4xNzA1Mzc0NzE0*_ga_NCQ7VMMZNQ*MTcwNjY4MTAzNi43LjEuMTcwNjY4MzkzOS4wLjAuMA..).


# [Deployment Options](https://go2docs.graylog.org/5-2/planning_your_deployment/deployment_options.html?tocpath=Planning%20Your%20Deployment%7C_____2)

## Containers

Graylog supports Docker for the deployment of Graylog, MongoDB, and Elasticsearch. Please see the [Docker](https://go2docs.graylog.org/5-2/downloading_and_installing_graylog/docker_installation.htm) installation page for instructions.


# [Initial Configuration Settings](https://go2docs.graylog.org/5-2/setting_up_graylog/initial_configuration_settings.html?tocpath=Setting%20up%20Graylog%7CGetting%20Started%7CInitial%20Configuration%20Settings%7C_____0)

Configurations must be set for Graylog to start after installation. Both the Graylog `server.conf` and Elasticsearch `elasticsearch.yml` configuration files contain the key details needed for initial configuration.

## server.conf

The file `server.conf` is the Graylog configuration file. The default location for `server.conf` is: `/etc/graylog/server/server.conf`.

> Web Properties

* `http_bind_address = 127.0.0.1:9000`
  + The network interface used by the Graylog HTTP interface.
  + This address and port is used by default in the `http_publish_uri`.

> Elasticsearch Properties

* `elasticsearch_hosts = https://node1:9200,https://user:password@node2:19200`
  + List of Elasticsearch hosts Graylog should connect to.
  + Need to be specified as a comma-separated list of valid URIs for the HTTP ports of Elasticsearch nodes.
  + If one or more Elasticsearch hosts require authentication, include the credentials in each node URI that requires authentication.
  + Default: `https://127.0.0.1:9200` You may retain the default setting only if Elasticsearch is installed on the same host as the Graylog server.

> MongoDB

* `mongodb_uri = mongdb://...`
  + MongoDB connection string. Enter your MongoDB connection and authentication information here.
  + See https://docs.mongodb.com/manual/reference/connection-string/ for details.
  + Examples:
    - Simple: `mongodb_uri = mongodb://localhost/graylog`
    - Authenticate against the MongoDB server: `mongodb_uri = mongodb://grayloguser:secret@localhost:27017/graylog`
    - Use a replica set instead of a single host: `mongodb_uri=mongodb://grayloguser:secret@localhost:27017,localhost:27018,localhost:27019/graylog?replicaSet=rs01`

## elasticsearch.yml

`Elasticsearch.yml` is the Elasticsearch configuration file. The default location for Elasticsearch.yml is: `/etc/elasticsearch/elasticsearch.yml`.

Several values must be properly configured for Elasticsearch to work properly.

* `cluster.name: graylog`
  + This value may be set to anything the customer wishes, though we recommend using "graylog".
  + This value must be the same for every Elasticsearch node in a cluster.
* `network.host: 172.30.4.105`
  + By default, Elasticsearch only binds to loop-back addresses (e.g., 127.0.0.1). This is sufficient to run a single development node on a server.
  + To communicate and form a cluster with nodes on other servers, the node will need to bind to a non-loopback address.
* `http.port: 9200`
  + Port Elasticsearch will listen on. We recommend the default value.
* `discovery.zen.ping.unicast.hosts: ["es01.acme.org", "es02.acme.org"]`
  + Elasticsearch uses a custom discovery implementation called "Zen Discovery" for node-to-node clustering and leader election. To form a cluster with nodes on other servers, provide a seed list of other nodes in the cluster that are likely to be live and contactable.
  + May be specified as an IP address or FQDN.



# [Index Model](https://go2docs.graylog.org/5-2/setting_up_graylog/index_model.html?tocpath=Setting%20up%20Graylog%7CGetting%20Started%7CIndex%20Model%7C_____0)

Graylog transparently manages one or more sets of Elasticsearch/OpenSearch indices to optimize search and analysis operations for speed and low resource consumption.

To enable managing indices with different [mappings](https://www.elastic.co/guide/en/elasticsearch/reference/7.10/mapping.html), [analyzers](https://www.elastic.co/guide/en/elasticsearch/reference/7.10/configure-text-analysis.html), and [replication settings](https://www.elastic.co/guide/en/elasticsearch/reference/7.10/index-modules.html), Graylog uses index sets, which are an abstraction of all these settings. For information on index default configuration settings, see [Index Defaults](https://go2docs.graylog.org/5-2/setting_up_graylog/index_defaults.htm).


# [Rest API](https://go2docs.graylog.org/5-2/setting_up_graylog/rest_api.html?tocpath=Setting%20up%20Graylog%7CGetting%20Started%7C_____5)

The functionality Graylog REST API is very comprehensive; even the Graylog web interface is exclusively using Graylog REST API to interact with the Graylog cluster.

To connect to the Graylog REST API with a web browser, just add `api/api-browser` to your current `http_publish_uri` setting or use the API browser button on the nodes overview page (System / Nodes in the web interface).

For example if your Graylog REST API is listening on `https://192.168.178.26:9000/api/`, the API browser will be available at `https://192.168.178.26:9000/api/api-browser/`.


# [GELF](https://go2docs.graylog.org/5-2/getting_in_log_data/gelf.html?tocpath=Getting%20in%20Logs%7CLog%20Sources%7CGELF%7C_____0)

The Graylog Extended Log Format (GELF) is a log format that avoids the shortcomings of classic plain syslog:

* Limited to length of 1024 bytes. Inadequate space for payloads like backtraces.
* No data types in structured syslog. Numbers and strings are indistinguishable.
* The RFCs are strict enough, but there are so many syslog dialects out there that you cannot possibly parse all of them.
* No compression.

Syslog is sufficient for logging system messages of machines or network gear, while GELF is a strong choice for logging from within applications. There are libraries and appenders for many programming languages and logging frameworks, so it is easy to implement. GELF can send every exception as a log message to your Graylog cluster without complications from timeouts, connection problems, or anything that may break your application from within your logging class because GELF can be sent via UDP.

## GELF Payload Specification

![gelf_payload_specification](/assets/images/202401/gelf_payload_specification.png)

## Example Payload

This is an example GELF message payload. Any Graylog-server node accepts and stores this as a message when GZIP/ZLIB is compressed or even when sent uncompressed over a plain socket without new lines.

> **Hint**: New lines must be denoted with the \n escape sequence to ensure the payload is valid JSON as per [RFC 7159](https://tools.ietf.org/html/rfc7159#page-8).

``` json
{
    "version": "1.1",
    "host": "example.org",
    "short_message": "A short message that helps you identify what is going on",
    "full_message": "Backtrace here\n\nmore stuff",
    "timestamp": 1385053862.3072,
    "level": 1,
    "_user_id": 9001,
    "_some_info": "foo",
    "_some_env_var": "bar"
}
```

> **Hint**: Currently, the server implementation of GELF in Graylog does not support boolean values. Boolean values will be dropped on ingest ([for reference](https://github.com/Graylog2/graylog2-server/issues/5504)).

## Sending GELF Messages via UDP Using Netcat

Sending an example message to a GELF UDP input (running on host graylog.example.com on port 12201):

``` bash
echo -n '{ "version": "1.1", "host": "example.org", "short_message": "A short message", "level": 5, "_some_info": "foo" }' | nc -w0 -u graylog.example.com 12201
```

## Sending GELF Messages via TCP Using Netcat

Sending an example message to a GELF TCP input (running on host graylog.example.com on port 12201):

``` bash
echo -n -e '{ "version": "1.1", "host": "example.org", "short_message": "A short message", "level": 5, "_some_info": "foo" }'"\0" | nc -w0 graylog.example.com 12201
```

## Sending GELF Messages Using Curl

Sending an example message to a GELF input (running on https://graylog.example.com:12201/gelf):

``` bash
curl -X POST -H 'Content-Type: application/json' -d '{ "version": "1.1", "host": "example.org", "short_message": "A short message", "level": 5, "_some_info": "foo" }' 'http://graylog.example.com:12201/gelf'
```

# [Graylog Sidecar](https://go2docs.graylog.org/5-2/getting_in_log_data/graylog_sidecar.html?tocpath=Getting%20in%20Logs%7CGraylog%20Sidecar%7C_____0)

Graylog Sidecar is a lightweight configuration management system for log collectors, which are also called backends. These collectors ingest data through inputs. An input can be a log file that the collector continuously reads or a connection to the Windows event system that emits log events. The Graylog node(s) acts as a centralized hub containing the configurations of log collectors. On supported message-producing devices/hosts, sidecar can run as a service (Windows host) or daemon (Linux host).

![log_collector_diagram](/assets/images/202401/log_collector_diagram.png)

Log collector configurations are centrally managed through the Graylog web interface. Periodically, the sidecar daemon will fetch all relevant configurations for the target, using the [REST API](https://go2docs.graylog.org/5-0/setting_up_graylog/rest_api.html?Highlight=rest%20api). On its first run or when a configuration change has been detected, sidecar will generate (render) relevant backend configuration files. Then it will start or restart those reconfigured log collectors.


# [Searching Your Log Data](https://go2docs.graylog.org/5-2/making_sense_of_your_log_data/how_to_search_your_log_data.html?tocpath=Searching%20Your%20Log%20Data%7C_____0)

The search page serves as the central hub of Graylog, where you can execute searches (queries) and visualize the results using a wide range of [widgets](https://go2docs.graylog.org/5-2/interacting_with_your_log_data/widgets.html). Any search can be saved or exported as a dashboard, allowing for easy reuse of specific search configurations. Dashboards offer the flexibility of widget-specific search queries and can be shared with others to enhance their work-flows. To further enhance the workflow, [parameters](https://go2docs.graylog.org/5-2/interacting_with_your_log_data/parameters.html) can also be incorporated into the search query.

![how_to_search_your_log_data](/assets/images/202401/how_to_search_your_log_data.png)





# [Frequently Asked Questions](https://go2docs.graylog.org/5-2/planning_your_deployment/faq.html?tocpath=Planning%20Your%20Deployment%7C_____3)

## What is MongoDB used for?

Graylog uses MongoDB to store your configuration data, not your log data. Only metadata is stored, such as user information or stream configurations. None of your log messages are ever stored in MongoDB. This is why MongoDB does not have a big system impact, and you won't have to worry too much about scaling it. With our recommended setup architecture, MongoDB will simply run alongside your graylog-server processes and use almost no resources.

## Does Graylog encrypt log data?

All log data is stored in Elasticsearch/OpenSearch. [Elastic recommends](https://discuss.elastic.co/t/how-should-i-encrypt-data-at-rest-with-elasticsearch/96) you use `dm-crypt` at the file system level.

## Where are the log files Graylog produces?

You can find the log data for Graylog under the below directory with timestamps and levels and exception messages. This is useful for debugging or when the server won't start.

``` bash
/var/log/graylog-server/server.log
```

If you use the pre-build appliances, take a look into

``` bash
/var/log/graylog/<servicename>/current
```


## How do I find out if a specific log source is supported?

We support many log sources - and more are coming everyday. For a complete list, check out Graylog Marketplace, the central repository of Graylog extensions. There are 4 types of content on the Marketplace:

* Plug-Ins: Code that extends Graylog to support a specific use case that it doesn't support out of the box.

* Content Pack: A file that can be uploaded into your Graylog system that sets up streams, inputs, extractors, dashboards, etc. to support a given log source or use case.

* GELF Library: A library for a programming language or logging framework that supports sending log messages in GELF format for easy integration and pre-structured messages.

* Other Solutions: Any other content or guide that helps you integrate Graylog with an external system or device. For example, how to configure a specific device to support a format Graylog understands out of the box.


## Can Graylog automatically clean old data?

Absolutely we have [data retention features](https://go2docs.graylog.org/5-0/setting_up_graylog/index_model.html).

## Can I filter inbound messages before they are processed by the Graylog server?

Yes, check out our page on how to use [blacklisting](https://go2docs.graylog.org/5-https://go2docs.graylog.org/5-0/making_sense_of_your_log_data/functions_descriptions.html#dropmessage).


## What is the best way to integrate my applications to Graylog?

We recommend that you use [GELF](https://go2docs.graylog.org/5-2/getting_in_log_data/gelf.html). It's easy for your application developers and eliminates the need to store the messages locally. Also, GELF can just send what app person wants so you don't have to build extractors or do any extra processing in Graylog.

## I don't want to use Elasticsearch/OpenSearch as my backend storage system - can I use another database, like MySQL, Oracle, etc?

Currently you can't. We only recommend another database if you want it for secondary storage.


## I'm sending in messages, and I can see they are being accepted by Graylog, but I can't see them in the search. What is going wrong?

A common reason for this issue is that the timestamp in the message is wrong. First, confirm that the message was received by selecting `all messages` as the time range for your search. Then identify and fix the source that is sending the wrong timestamp.




# Refer

* https://go2docs.graylog.org/5-2/what_is_graylog/what_is_graylog.htm
* https://github.com/Graylog2/graylog2-server











