---
layout: post
title:  "Telegraf in Action"
date:   2024-01-16 20:22:00 +0800
categories: 云原生
---

* Do not remove this line (it will not be displayed)
{:toc}


# Telegraf (The plugin-driven server agent for collecting & reporting metrics)


## [Intro to Telegraf](https://docs.influxdata.com/resources/videos/intro-to-telegraf/)

Telegraf is an open-source data collection agent by InfluxData.

Telegraf is a plugin-driven server agent for collecting and sending metrics and events from databases, systems, and IoT sensors. Telegraf is written in Go and compiles into a single binary with no external dependencies, and requires a very minimal memory footprint.

With 200+ plugins already written by subject matter experts on the data in the community, it is easy to start collecting metrics from your endpoints.

For an introduction to Telegraf and an overview of how it works, watch the following video:

https://youtu.be/vGJeo3FaMds

Telegraf is an agent for collecting, processing, aggregating, and writing metrics. Based on a plugin system to enable developers in the community to easily add support for additional metric collection. There are four distinct types of plugins:

* [Input Plugins](https://github.com/influxdata/telegraf/blob/master/docs/INPUTS.md) collect metrics from the system, services, or 3rd party APIs
* [Processor Plugins](https://github.com/influxdata/telegraf/blob/master/docs/PROCESSORS.md) transform, decorate, and/or filter metrics
* [Aggregator Plugins](https://github.com/influxdata/telegraf/blob/master/docs/AGGREGATORS.md) create aggregate metrics (e.g. mean, min, max, quantiles, etc.)
* [Output Plugins](https://github.com/influxdata/telegraf/blob/master/docs/OUTPUTS.md) write metrics to various destinations


```
data source |   inputs -> processors -> aggregators -> outputs   | InfluxDB
```




## [Install Telegraf](https://docs.influxdata.com/telegraf/v1/install/)

This page provides directions for installing, starting, and configuring Telegraf. To install Telegraf, do the following:

### [Download Telegraf](https://docs.influxdata.com/telegraf/v1/install/#download)

``` bash
wget https://dl.influxdata.com/telegraf/releases/telegraf-1.29.2_linux_amd64.tar.gz
tar xf telegraf-1.29.2_linux_amd64.tar.gz
```

For additional architecture (e.g. i386, riscv64, etc.) and operating system (e.g BSD, etc.) downloads please see the [Telegraf GitHub Releases](https://github.com/influxdata/telegraf/releases) page.

### Requirements

Installation of the Telegraf package may require `root` or administrator privileges to complete successfully.

### Networking

Telegraf offers multiple service [input plugins](https://docs.influxdata.com/telegraf/v1/plugins/inputs/) that may require custom ports. Modify port mappings through the configuration file (`telegraf.conf`).

For Linux distributions, this file is located at `/etc/telegraf` for default installations.

### NTP

Telegraf uses a host's local time in UTC to assign timestamps to data. Use the Network Time Protocol (NTP) to synchronize time between hosts. If hosts’ clocks aren't synchronized with NTP, the timestamps on the data might be inaccurate.

### Generate a custom configuration file

The `telegraf config` command lets you generate a configuration file using Telegraf’s list of plugins.

``` bash
telegraf config > telegraf.conf
```

The generated file contains settings for all available plugins–some are enabled and the rest are commented out.

### Create a configuration file with specific input and output plugins

To generate a configuration file that contains settings for only specific input and output plugins, specify telegraf plugin filters–for example:

``` bash
telegraf \
--input-filter <pluginname>[:<pluginname>] \
--output-filter <outputname>[:<outputname>] \
config > telegraf.conf
```

For more advanced configuration details, see the [configuration documentation](https://docs.influxdata.com/telegraf/v1/administration/configuration/).

### Custom-compile Telegraf

Use the Telegraf custom builder tool to compile Telegraf with only the plugins you need and reduce the Telegraf binary size.

#### Prerequisites

* Follow the instructions to install [Go](https://go.dev/) for your system.
* [Create your Telegraf configuration file](https://docs.influxdata.com/telegraf/v1/install/#generate-a-custom-configuration-file) with the plugins you want to use.

#### Build the custom builder tool

* Clone the Telegraf repository

```
git clone https://github.com/influxdata/telegraf.git
```

* Change directories into the top-level of the Telegraf repository

```
cd telegraf
```

* Build the Telegraf custom builder tool by entering the following command

```
make build_tools
```

#### Run the custom builder to create a telegraf binary

The custom builder builds a `telegraf` binary with only the plugins included in the specified configuration files or directories.

Run the `custom_builder` tool with at least one `--config` or `--config-directory` flag to specify Telegraf configuration files to build from.

* `--config`: accepts local file paths and URLs.
* `--config-dir`: accepts local directory paths.

You can include multiple `--config` and `--config-dir` flags.

Examples

``` bash
# Single Telegraf configuration
./tools/custom_builder/custom_builder --config /etc/telegraf.conf
```

``` bash
# Single Telegraf configuration and Telegraf configuration directory
./tools/custom_builder/custom_builder \
--config /etc/telegraf.conf \
--config-dir /etc/telegraf/telegraf.d
```

``` bash
# Remote Telegraf configuration
./tools/custom_builder/custom_builder \
--config http://url-to-remote-telegraf/telegraf.conf
```

After a successful build, you can view your customized telegraf binary within the top level of your Telegraf repository.

#### Update your custom binary

To add or remove plugins from your customized Telegraf build, edit your configuration file, and then run the custom builder to regenerate the Telegraf binary.



## Get started

After you've downloaded and installed Telegraf, you're ready to begin collecting and sending data. To collect and send data, do the following:

### Configure Telegraf

Define which plugins Telegraf will use in the configuration file. Each configuration file needs at least one enabled [input plugin](https://docs.influxdata.com/telegraf/v1/plugins/inputs/) (where the metrics come from) and at least one enabled [output plugin](https://docs.influxdata.com/telegraf/v1/plugins/outputs/) (where the metrics go).

The following example generates a sample configuration file with all available plugins, then uses `filter` flags to enable specific plugins.

> For details on `filter` and other flags, see [Telegraf commands and flags](https://docs.influxdata.com/telegraf/v1/commands/).


* Run the following command to create a configuration file

``` bash
telegraf --sample-config > telegraf.conf
```

* Locate the configuration file. The location varies depending on your system


1. Linux debian and RPM packages: `/etc/telegraf/telegraf.conf`
2. Standalone Binary: see the next section for how to create a configuration file

> Note: You can also specify a remote URL endpoint to pull a configuration file from. See [Configuration file locations](https://docs.influxdata.com/telegraf/v1/configuration/#configuration-file-locations).

* Edit the configuration file using `vim` or a text editor. Because this example uses [InfluxDB V2 output plugin](https://github.com/influxdata/telegraf/blob/release-1.21/plugins/outputs/influxdb_v2/README.md), we need to add the InfluxDB URL, authentication token, organization, and bucket details to this section of the configuration file.

> Note: For more configuration file options, see [Configuration options](https://docs.influxdata.com/telegraf/v1/configuration/).

For this example, specify two inputs (`cpu` and `mem`) with the `--input-filter` flag. Specify InfluxDB as the output with the `--output-filter` flag.

``` bash
telegraf --sample-config --input-filter cpu:mem --output-filter influxdb_v2 > telegraf.conf
```

The resulting configuration will collect CPU and memory data and sends it to InfluxDB V2.


``` bash
$ ./telegraf -config telegraf.conf -test
2024-01-17T03:55:55Z I! Loading config: telegraf.conf
2024-01-17T03:55:55Z I! Starting Telegraf 1.29.2 brought to you by InfluxData the makers of InfluxDB
2024-01-17T03:55:55Z I! Available plugins: 241 inputs, 9 aggregators, 30 processors, 24 parsers, 60 outputs, 6 secret-stores
2024-01-17T03:55:55Z I! Loaded inputs: cpu mem
2024-01-17T03:55:55Z I! Loaded aggregators:
2024-01-17T03:55:55Z I! Loaded processors:
2024-01-17T03:55:55Z I! Loaded secretstores:
2024-01-17T03:55:55Z W! Outputs are not used in testing mode!
2024-01-17T03:55:55Z I! Tags enabled: host=VM-129-173-tencentos
> mem,host=VM-129-173-tencentos active=70480793600i,available=88298409984i,available_percent=65.58129763069887,buffered=402878464i,cached=51528310784i,commit_limit=67319808000i,committed_as=50656702464i,dirty=1343488i,free=37684277248i,high_free=0i,high_total=0i,huge_page_size=2097152i,huge_pages_free=0i,huge_pages_total=0i,inactive=22407303168i,low_free=0i,low_total=0i,mapped=1698852864i,page_tables=129990656i,shared=399167488i,slab=3315363840i,sreclaimable=2334392320i,sunreclaim=980971520i,swap_cached=0i,swap_free=0i,swap_total=0i,total=134639620096i,used=45024153600i,used_percent=33.44049364362223,vmalloc_chunk=0i,vmalloc_total=35184372087808i,vmalloc_used=151322624i,write_back=0i,write_back_tmp=0i 1705463755000000000
> cpu,cpu=cpu0,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=97.95918372001613,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=2.040816324530557 1705463756000000000
> cpu,cpu=cpu1,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=97.95918353382919,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=2.040816322136552 1705463756000000000
> cpu,cpu=cpu2,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=95.99999990314245,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=2.0000000000436557,usage_user=1.999999998952262 1705463756000000000
> cpu,cpu=cpu3,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=95.91836744003224,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=4.081632650546004 1705463756000000000
> cpu,cpu=cpu4,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=94.23076935476188,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=1.9230769244223294,usage_user=3.8461538488446587 1705463756000000000
> cpu,cpu=cpu5,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=95.91836743227445,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=4.081632659788682 1705463756000000000
> cpu,cpu=cpu6,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=92.15686273435614,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=1.9607843142709773,usage_user=5.882352943526261 1705463756000000000
> cpu,cpu=cpu7,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=95.91836743227445,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=2.040816329523119,usage_user=2.0408163284094516 1705463756000000000
> cpu,cpu=cpu8,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=93.87755096998254,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=6.122448976561451 1705463756000000000
> cpu,cpu=cpu9,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=96.00000008940697,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=2.0000000000436557,usage_user=2.0000000004074536 1705463756000000000
> cpu,cpu=cpu10,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=95.91836744003224,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=4.081632650546004 1705463756000000000
> cpu,cpu=cpu11,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=93.87755096998254,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=6.122448976561451 1705463756000000000
> cpu,cpu=cpu12,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=97.95918372001613,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=2.0408163260154466 1705463756000000000
> cpu,cpu=cpu13,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=96.00000008940697,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=3.9999999993597157 1705463756000000000
> cpu,cpu=cpu14,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=97.91666671113086,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=2.0833333358597077 1705463756000000000
> cpu,cpu=cpu15,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=93.9999999590218,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=1.9999999959545676,usage_user=3.9999999933643267 1705463756000000000
> cpu,cpu=cpu16,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=97.91666652114752,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=2.0833333318175087 1705463756000000000
> cpu,cpu=cpu17,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=97.91666671113086,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=2.0833333358597077 1705463756000000000
> cpu,cpu=cpu18,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=97.95918371613723,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=2.040816329894341 1705463756000000000
> cpu,cpu=cpu19,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=100,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=0 1705463756000000000
> cpu,cpu=cpu20,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=95.91836743227445,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=2.040816329523119,usage_user=2.040816329894341 1705463756000000000
> cpu,cpu=cpu21,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=93.9999999590218,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=1.9999999963183654,usage_user=3.9999999933643267 1705463756000000000
> cpu,cpu=cpu22,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=94.00000013411045,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=2.0000000000436557,usage_user=3.9999999993597157 1705463756000000000
> cpu,cpu=cpu23,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=95.91836724996641,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=4.081632652030893 1705463756000000000
> cpu,cpu=cpu24,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=97.99999986216426,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=1.9999999966821633 1705463756000000000
> cpu,cpu=cpu25,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=97.99999986216426,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=1.9999999966821633 1705463756000000000
> cpu,cpu=cpu26,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=97.95918372001613,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=2.0408163260154466 1705463756000000000
> cpu,cpu=cpu27,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=96.00000008940697,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=2.0000000000436557,usage_user=2.0000000004074536 1705463756000000000
> cpu,cpu=cpu28,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=96.00000008940697,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=2.0000000000436557,usage_user=2.0000000004074536 1705463756000000000
> cpu,cpu=cpu29,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=95.91836724996641,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=4.081632652030893 1705463756000000000
> cpu,cpu=cpu30,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=97.9591835299503,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=2.0408163260154466 1705463756000000000
> cpu,cpu=cpu31,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=93.99999994784594,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=2.0000000000436557,usage_user=4.000000000814907 1705463756000000000
> cpu,cpu=cpu32,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=95.91836744003224,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=4.081632650546004 1705463756000000000
> cpu,cpu=cpu33,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=95.91836744003224,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=2.0408163256442244,usage_user=2.0408163260154466 1705463756000000000
> cpu,cpu=cpu34,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=97.95918372001613,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=2.0408163256442244,usage_user=0 1705463756000000000
> cpu,cpu=cpu35,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=95.8333334222617,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=4.166666670203591 1705463756000000000
> cpu,cpu=cpu36,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=93.87755114841168,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=6.122448988198134 1705463756000000000
> cpu,cpu=cpu37,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=91.99999999254942,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=1.9999999996798579,usage_user=5.999999999767169 1705463756000000000
> cpu,cpu=cpu38,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=93.75000013339256,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=6.250000006063298 1705463756000000000
> cpu,cpu=cpu39,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=93.87755096998254,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=6.122448976561451 1705463756000000000
> cpu,cpu=cpu40,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=94.00000012293458,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=2.000000003768946,usage_user=4.000000006810296 1705463756000000000
> cpu,cpu=cpu41,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=97.95918371613723,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=2.040816329894341 1705463756000000000
> cpu,cpu=cpu42,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=97.95918371613723,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=2.040816329894341 1705463756000000000
> cpu,cpu=cpu43,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=100,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=0 1705463756000000000
> cpu,cpu=cpu44,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=100,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=0 1705463756000000000
> cpu,cpu=cpu45,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=100,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=0 1705463756000000000
> cpu,cpu=cpu46,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=100,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=0 1705463756000000000
> cpu,cpu=cpu47,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=97.99999986216426,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0,usage_user=1.9999999966821633 1705463756000000000
> cpu,cpu=cpu-total,host=VM-129-173-tencentos usage_guest=0,usage_guest_nice=0,usage_idle=96.23996626029599,usage_iowait=0,usage_irq=0,usage_nice=0,usage_softirq=0,usage_steal=0,usage_system=0.7182087033286046,usage_user=3.0418250966241467 1705463756000000000
```

For an overview of how to configure a plugin, watch the following video: https://youtu.be/a0js7wiQEJ4

### Set environment variables

Add environment variables anywhere in the configuration file by prepending them with `$`. For strings, variables must be in quotes (for example, `"$STR_VAR"`). For numbers and Booleans, variables must be unquoted (for example, `$INT_VAR`, `$BOOL_VAR`).

You can also set environment variables using the Linux `export` command: `export password=mypassword`

> Note: We recommend using environment variables for sensitive information.

Example: Telegraf environment variables

In the Telegraf environment variables file (`/etc/default/telegraf`):

```
USER="alice"
INFLUX_URL="https://us-west-2-1.aws.cloud2.influxdata.com"
INFLUX_SKIP_DATABASE_CREATION="true"
INFLUX_PASSWORD="monkey123"
```

In the Telegraf configuration file (`/etc/telegraf.conf`):

``` bash
[global_tags]
  user = "${USER}"

[[inputs.mem]]

[[outputs.influxdb]]
  urls = ["${INFLUX_URL}"]
  skip_database_creation = ${INFLUX_SKIP_DATABASE_CREATION}
  password = "${INFLUX_PASSWORD}"
```

The environment variables above add the following configuration settings to Telegraf:

``` bash
[global_tags]
  user = "alice"

[[outputs.influxdb]]
  urls = "https://us-west-2-1.aws.cloud2.influxdata.com"
  skip_database_creation = true
  password = "monkey123"
```


### Start Telegraf

Next, you need to start the Telegraf service and direct it to your configuration file:

Linux (systemd installations)

```
systemctl start telegraf
```



### Use plugins available in Telegraf to gather, transform, and output data.


[Plugin directory](https://docs.influxdata.com/telegraf/v1/plugins/)

#### input

```
$ ./telegraf --input-list
DEPRECATED: use telegraf plugins inputs
Available Input Plugins:
  KNXListener
  activemq
  aerospike
  aliyuncms
  amd_rocm_smi
  amqp_consumer
  apache
  apcupsd
  aurora
  azure_monitor
  azure_storage_queue
  bcache
  beanstalkd
  beat
  bind
  bond
  burrow
  cassandra
  ceph
  cgroup
  chrony
  cisco_telemetry_gnmi
  cisco_telemetry_mdt
  clickhouse
  cloud_pubsub
  cloud_pubsub_push
  cloudwatch
  cloudwatch_metric_streams
  conntrack
  consul
  consul_agent
  couchbase
  couchdb
  cpu
  csgo
  ctrlx_datalayer
  dcos
  directory_monitor
  disk
  diskio
  disque
  dmcache
  dns_query
  docker
  docker_log
  dovecot
  dpdk
  ecs
  elasticsearch
  elasticsearch_query
  ethtool
  eventhub_consumer
  exec
  execd
  fail2ban
  fibaro
  file
  filecount
  filestat
  fireboard
  fluentd
  github
  gnmi
  google_cloud_storage
  graylog
  haproxy
  hddtemp
  http
  http_listener
  http_listener_v2
  http_response
  httpjson
  hugepages
  icinga2
  infiniband
  influxdb
  influxdb_listener
  influxdb_v2_listener
  intel_baseband
  intel_dlb
  intel_pmt
  intel_pmu
  intel_powerstat
  intel_rdt
  internal
  internet_speed
  interrupts
  io
  ipmi_sensor
  ipset
  iptables
  ipvs
  jenkins
  jolokia
  jolokia2_agent
  jolokia2_proxy
  jti_openconfig_telemetry
  kafka_consumer
  kafka_consumer_legacy
  kapacitor
  kernel
  kernel_vmstat
  kibana
  kinesis_consumer
  knx_listener
  kube_inventory
  kubernetes
  lanz
  ldap
  leofs
  libvirt
  linux_cpu
  linux_sysctl_fs
  logparser
  logstash
  lustre2
  lvm
  mailchimp
  marklogic
  mcrouter
  mdstat
  mem
  memcached
  mesos
  minecraft
  mock
  modbus
  mongodb
  monit
  mqtt_consumer
  multifile
  mysql
  nats
  nats_consumer
  neptune_apex
  net
  net_response
  netflow
  netstat
  nfsclient
  nginx
  nginx_plus
  nginx_plus_api
  nginx_sts
  nginx_upstream_check
  nginx_vts
  nomad
  nsd
  nsq
  nsq_consumer
  nstat
  ntpq
  nvidia_smi
  opcua
  opcua_listener
  openldap
  openntpd
  opensearch_query
  opensmtpd
  openstack
  opentelemetry
  openweathermap
  p4runtime
  passenger
  pf
  pgbouncer
  phpfpm
  ping
  postfix
  postgresql
  postgresql_extensible
  powerdns
  powerdns_recursor
  processes
  procstat
  prometheus
  proxmox
  puppetagent
  rabbitmq
  radius
  raindrops
  ras
  ravendb
  redfish
  redis
  redis_sentinel
  rethinkdb
  riak
  riemann_listener
  s7comm
  salesforce
  sensors
  sflow
  slab
  smart
  snmp
  snmp_legacy
  snmp_trap
  socket_listener
  socketstat
  solr
  sql
  sqlserver
  stackdriver
  statsd
  supervisor
  suricata
  swap
  synproxy
  syslog
  sysstat
  system
  systemd_units
  tacacs
  tail
  tcp_listener
  teamspeak
  temp
  tengine
  tomcat
  trig
  twemproxy
  udp_listener
  unbound
  upsd
  uwsgi
  varnish
  vault
  vsphere
  webhooks
  win_eventlog
  win_perf_counters
  win_services
  win_wmi
  wireguard
  wireless
  x509_cert
  xtremio
  zfs
  zipkin
  zookeeper
```

#### output


```
$ ./telegraf --output-list
DEPRECATED: use telegraf plugins outputs
Available Output Plugins:
  amon
  amqp
  application_insights
  azure_data_explorer
  azure_monitor
  bigquery
  clarify
  cloud_pubsub
  cloudwatch
  cloudwatch_logs
  cratedb
  datadog
  discard
  dynatrace
  elasticsearch
  event_hubs
  exec
  execd
  file
  graphite
  graylog
  groundwork
  health
  http
  influxdb
  influxdb_v2
  instrumental
  iotdb
  kafka
  kinesis
  librato
  logzio
  loki
  mongodb
  mqtt
  nats
  nebius_cloud_monitoring
  newrelic
  nsq
  opensearch
  opentelemetry
  opentsdb
  postgresql
  prometheus_client
  redistimeseries
  riemann
  riemann_legacy
  sensu
  signalfx
  socket_writer
  sql
  stackdriver
  stomp
  sumologic
  syslog
  timestream
  warp10
  wavefront
  websocket
  yandex_cloud_monitoring
```

#### Input Data Formats

https://github.com/influxdata/telegraf/blob/master/docs/DATA_FORMATS_INPUT.md

Telegraf contains many general purpose plugins that support parsing input data using a configurable parser into [metrics](https://github.com/influxdata/telegraf/blob/master/docs/METRICS.md). This allows, for example, the `kafka_consumer` input plugin to process messages in any of InfluxDB Line Protocol, JSON format, or Apache Avro format.

Any input plugin containing the `data_format` option can use it to select the desired parser:

``` bash
[[inputs.exec]]
  ## Commands array
  commands = ["/tmp/test.sh", "/usr/bin/mycollector --foo=bar"]

  ## measurement name suffix (for separating different commands)
  name_suffix = "_mycollector"

  ## Data format to consume.
  data_format = "json"
```



#### file (input / output)

```
$ ./telegraf --usage file

# Parse a complete file each interval
[[inputs.file]]
  ## Files to parse each interval.  Accept standard unix glob matching rules,
  ## as well as ** to match recursive files and directories.
  files = ["/tmp/metrics.out"]

  ## Character encoding to use when interpreting the file contents.  Invalid
  ## characters are replaced using the unicode replacement character.  When set
  ## to the empty string the data is not decoded to text.
  ##   ex: character_encoding = "utf-8"
  ##       character_encoding = "utf-16le"
  ##       character_encoding = "utf-16be"
  ##       character_encoding = ""
  # character_encoding = ""

  ## Data format to consume.
  ## Each data format has its own unique set of configuration options, read
  ## more about them here:
  ## https://github.com/influxdata/telegraf/blob/master/docs/DATA_FORMATS_INPUT.md
  data_format = "influx"


  ## Name a tag containing the name of the file the data was parsed from.  Leave empty
  ## to disable. Cautious when file name variation is high, this can increase the cardinality
  ## significantly. Read more about cardinality here:
  ## https://docs.influxdata.com/influxdb/cloud/reference/glossary/#series-cardinality
  # file_tag = ""


# Send telegraf metrics to file(s)
[[outputs.file]]
  ## Files to write to, "stdout" is a specially handled file.
  files = ["stdout", "/tmp/metrics.out"]

  ## Use batch serialization format instead of line based delimiting.  The
  ## batch format allows for the production of non line based output formats and
  ## may more efficiently encode and write metrics.
  # use_batch_format = false

  ## The file will be rotated after the time interval specified.  When set
  ## to 0 no time based rotation is performed.
  # rotation_interval = "0h"

  ## The logfile will be rotated when it becomes larger than the specified
  ## size.  When set to 0 no size based rotation is performed.
  # rotation_max_size = "0MB"

  ## Maximum number of rotated archives to keep, any older logs are deleted.
  ## If set to -1, no archives are removed.
  # rotation_max_archives = 5

  ## Data format to output.
  ## Each data format has its own unique set of configuration options, read
  ## more about them here:
  ## https://github.com/influxdata/telegraf/blob/master/docs/DATA_FORMATS_OUTPUT.md
  data_format = "influx"

  ## Compress output data with the specified algorithm.
  ## If empty, compression will be disabled and files will be plain text.
  ## Supported algorithms are "zstd", "gzip" and "zlib".
  # compression_algorithm = ""

  ## Compression level for the algorithm above.
  ## Please note that different algorithms support different levels:
  ##   zstd  -- supports levels 1, 3, 7 and 11.
  ##   gzip -- supports levels 0, 1 and 9.
  ##   zlib -- supports levels 0, 1, and 9.
  ## By default the default compression level for each algorithm is used.
  # compression_level = -1
```

这是 Telegraf 的一个配置示例，用于从文件中读取度量值（metrics）并将结果输出到另一个文件。配置分为两部分：inputs.file 和 outputs.file。

* inputs.file 插件用于从文件中解析度量值。以下是主要配置选项：
  + files：要解析的文件列表。可以使用标准的 Unix glob 匹配规则，以及 ** 用于匹配递归文件和目录。在此示例中，Telegraf 将解析 /tmp/metrics.out 文件。
  + character_encoding：用于解释文件内容的字符编码。无效字符将使用 Unicode 替换字符进行替换。当设置为空字符串时，数据不会解码为文本。默认为空。
  + data_format：要使用的数据格式。每种数据格式都有其独特的配置选项。在此示例中，使用的数据格式为 "influx"。有关不同数据格式的详细信息，请参阅 Telegraf 数据格式文档。
  + file_tag：包含从中解析数据的文件名的标签名称。留空以禁用。当文件名变化较大时要谨慎，这可能会显著增加基数。关于基数的更多信息，请参阅 InfluxDB 基数文档。


* outputs.file 插件用于将度量值写入文件。以下是主要配置选项：
  + files：要写入的文件列表。"stdout" 是一个特殊处理的文件。在此示例中，Telegraf 将将度量值写入 "stdout" 和 /tmp/metrics.out。
  + use_batch_format：使用批处理序列化格式而不是基于行的分隔。批处理格式允许生成非基于行的输出格式，并可能更有效地编码和写入度量值。
  + rotation_interval 和 rotation_max_size：用于配置日志文件轮换的时间间隔和最大大小。当达到指定的时间间隔或大小时，日志文件将被轮换。
  + rotation_max_archives：要保留的最大轮换存档数量。任何较旧的日志将被删除。如果设置为 -1，则不会删除任何存档。
  + data_format：输出的数据格式。在此示例中，使用的数据格式为 "influx"。有关不同数据格式的详细信息，请参阅 Telegraf 数据格式文档。
  + compression_algorithm 和 compression_level：用于压缩输出数据的算法和级别。支持的算法有 "zstd"、"gzip" 和 "zlib"。默认情况下，将使用各个算法的默认压缩级别。

通过运行 `./telegraf --usage file` 命令，可以查看 Telegraf 的 file 输入插件和输出插件的用法和配置选项。


#### tail (input) + file (output)


``` bash
# Stream a log file, like the tail -F command
[[inputs.tail]]
  ## files to tail.
  ## These accept standard unix glob matching rules, but with the addition of
  ## ** as a "super asterisk". ie:
  ##   "/var/log/**.log"  -> recursively find all .log files in /var/log
  ##   "/var/log/*/*.log" -> find all .log files with a parent dir in /var/log
  ##   "/var/log/apache.log" -> just tail the apache log file
  ##
  ## See https://github.com/gobwas/glob for more examples
  ##
  files = ["/var/log/apache/access.log"]
  ## Read file from beginning.
  from_beginning = false
  ## Whether file is a named pipe
  pipe = false
  ## Method used to watch for file updates.  Can be either "inotify" or "poll".
  watch_method = "inotify"
  ## Maximum lines of the file to process that have not yet be written by the
  ## output.  For best throughput set based on the number of metrics and the size of the output's metric_batch_size.
  max_undelivered_lines = 1000
  data_format = "influx"
```

Telegraf 的 inputs.file 插件默认行为是每次收集间隔都会读取并解析指定的文件。如果文件内容在两次收集间隔之间没有变化，那么 Telegraf 将会重复收集相同的数据。

要解决这个问题，你可以使用 tail 插件代替 file 插件。tail 插件的工作方式类似于 Unix/Linux 中的 tail -F 命令，它会持续追踪并读取指定文件的新内容。这意味着，只有当文件有新的数据添加时，tail 插件才会收集数据，从而避免了重复收集的问题。

在这个配置中，tail 插件将会追踪并读取 /var/log/apache/access.log 文件的新内容。from_beginning 设置为 false，这意味着 Telegraf 将从文件的末尾开始读取，忽略已经存在的内容。data_format 设置为 "influx"，表示数据格式为 InfluxDB 数据格式。


#### exec (input / output)

``` bash
$ ./telegraf --usage exec

# Read metrics from one or more commands that can output to stdout
[[inputs.exec]]
  ## Commands array
  commands = [
    "/tmp/test.sh",
    "/usr/bin/mycollector --foo=bar",
    "/tmp/collect_*.sh"
  ]

  ## Environment variables
  ## Array of "key=value" pairs to pass as environment variables
  ## e.g. "KEY=value", "USERNAME=John Doe",
  ## "LD_LIBRARY_PATH=/opt/custom/lib64:/usr/local/libs"
  # environment = []

  ## Timeout for each command to complete.
  timeout = "5s"

  ## measurement name suffix (for separating different commands)
  name_suffix = "_mycollector"

  ## Data format to consume.
  ## Each data format has its own unique set of configuration options, read
  ## more about them here:
  ## https://github.com/influxdata/telegraf/blob/master/docs/DATA_FORMATS_INPUT.md
  data_format = "influx"


# Send metrics to command as input over stdin
[[outputs.exec]]
  ## Command to ingest metrics via stdin.
  command = ["tee", "-a", "/dev/null"]

  ## Environment variables
  ## Array of "key=value" pairs to pass as environment variables
  ## e.g. "KEY=value", "USERNAME=John Doe",
  ## "LD_LIBRARY_PATH=/opt/custom/lib64:/usr/local/libs"
  # environment = []

  ## Timeout for command to complete.
  # timeout = "5s"

  ## Whether the command gets executed once per metric, or once per metric batch
  ## The serializer will also run in batch mode when this is true.
  # use_batch_format = true

  ## Data format to output.
  ## Each data format has its own unique set of configuration options, read
  ## more about them here:
  ## https://github.com/influxdata/telegraf/blob/master/docs/DATA_FORMATS_OUTPUT.md
  # data_format = "influx"
```

#### socket_listener (input)

```
$ ./telegraf --usage socket_listener

# Generic socket listener capable of handling multiple socket types.
[[inputs.socket_listener]]
  ## URL to listen on
  # service_address = "tcp://:8094"
  # service_address = "tcp://127.0.0.1:http"
  # service_address = "tcp4://:8094"
  # service_address = "tcp6://:8094"
  # service_address = "tcp6://[2001:db8::1]:8094"
  # service_address = "udp://:8094"
  # service_address = "udp4://:8094"
  # service_address = "udp6://:8094"
  # service_address = "unix:///tmp/telegraf.sock"
  # service_address = "unixgram:///tmp/telegraf.sock"
  # service_address = "vsock://cid:port"

  ## Change the file mode bits on unix sockets.  These permissions may not be
  ## respected by some platforms, to safely restrict write permissions it is best
  ## to place the socket into a directory that has previously been created
  ## with the desired permissions.
  ##   ex: socket_mode = "777"
  # socket_mode = ""

  ## Maximum number of concurrent connections.
  ## Only applies to stream sockets (e.g. TCP).
  ## 0 (default) is unlimited.
  # max_connections = 1024

  ## Read timeout.
  ## Only applies to stream sockets (e.g. TCP).
  ## 0 (default) is unlimited.
  # read_timeout = "30s"

  ## Optional TLS configuration.
  ## Only applies to stream sockets (e.g. TCP).
  # tls_cert = "/etc/telegraf/cert.pem"
  # tls_key  = "/etc/telegraf/key.pem"
  ## Enables client authentication if set.
  # tls_allowed_cacerts = ["/etc/telegraf/clientca.pem"]

  ## Maximum socket buffer size (in bytes when no unit specified).
  ## For stream sockets, once the buffer fills up, the sender will start backing up.
  ## For datagram sockets, once the buffer fills up, metrics will start dropping.
  ## Defaults to the OS default.
  # read_buffer_size = "64KiB"

  ## Period between keep alive probes.
  ## Only applies to TCP sockets.
  ## 0 disables keep alive probes.
  ## Defaults to the OS configuration.
  # keep_alive_period = "5m"

  ## Data format to consume.
  ## Each data format has its own unique set of configuration options, read
  ## more about them here:
  ## https://github.com/influxdata/telegraf/blob/master/docs/DATA_FORMATS_INPUT.md
  # data_format = "influx"

  ## Content encoding for message payloads, can be set to "gzip" to or
  ## "identity" to apply no encoding.
  # content_encoding = "identity"

  ## Maximum size of decoded packet.
  ## Acceptable units are B, KiB, KB, MiB, MB...
  ## Without quotes and units, interpreted as size in bytes.
  # max_decompression_size = "500MB"

  ## Message splitting strategy and corresponding settings for stream sockets
  ## (tcp, tcp4, tcp6, unix or unixpacket). The setting is ignored for packet
  ## listeners such as udp.
  ## Available strategies are:
  ##   newline         -- split at newlines (default)
  ##   null            -- split at null bytes
  ##   delimiter       -- split at delimiter byte-sequence in hex-format
  ##                      given in `splitting_delimiter`
  ##   fixed length    -- split after number of bytes given in `splitting_length`
  ##   variable length -- split depending on length information received in the
  ##                      data. The length field information is specified in
  ##                      `splitting_length_field`.
  # splitting_strategy = "newline"

  ## Delimiter used to split received data to messages consumed by the parser.
  ## The delimiter is a hex byte-sequence marking the end of a message
  ## e.g. "0x0D0A", "x0d0a" or "0d0a" marks a Windows line-break (CR LF).
  ## The value is case-insensitive and can be specified with "0x" or "x" prefix
  ## or without.
  ## Note: This setting is only used for splitting_strategy = "delimiter".
  # splitting_delimiter = ""

  ## Fixed length of a message in bytes.
  ## Note: This setting is only used for splitting_strategy = "fixed length".
  # splitting_length = 0

  ## Specification of the length field contained in the data to split messages
  ## with variable length. The specification contains the following fields:
  ##  offset        -- start of length field in bytes from begin of data
  ##  bytes         -- length of length field in bytes
  ##  endianness    -- endianness of the value, either "be" for big endian or
  ##                   "le" for little endian
  ##  header_length -- total length of header to be skipped when passing
  ##                   data on to the parser. If zero (default), the header
  ##                   is passed on to the parser together with the message.
  ## Note: This setting is only used for splitting_strategy = "variable length".
  # splitting_length_field = {offset = 0, bytes = 0, endianness = "be", header_length = 0}
```

## Usage

```
$ ./telegraf --version
Telegraf 1.29.2 (git: HEAD@d92d7073)

$ ./telegraf --help
NAME:
   Telegraf - The plugin-driven server agent for collecting & reporting metrics.

USAGE:
   Telegraf [global options] command [command options] [arguments...]

COMMANDS:
   version  print current version to stdout
   config   commands for generating and migrating configurations
   secrets  commands for listing, adding and removing secrets on all known secret-stores
   plugins  commands for printing available plugins
   help, h  Shows a list of commands or help for one command

GLOBAL OPTIONS:
   --config value [ --config value ]                      configuration file to load
   --config-directory value [ --config-directory value ]  directory containing additional *.conf files
   --test-wait value                                      wait up to this many seconds for service inputs to complete in test mode (default: 0)
   --usage value                                          print usage for a plugin, ie, 'telegraf --usage mysql'
   --pprof-addr value                                     pprof host/IP and port to listen on (e.g. 'localhost:6060')
   --watch-config value                                   monitoring config changes [notify, poll] of --config and --config-directory options
   --pidfile value                                        file to write our pid to
   --password value                                       password to unlock secret-stores
   --old-env-behavior                                     switch back to pre v1.27 environment replacement behavior (default: false)
   --once                                                 run one gather and exit (default: false)
   --debug                                                turn on debug logging (default: false)
   --quiet                                                run in quiet mode (default: false)
   --unprotected                                          do not protect secrets in memory (default: false)
   --test                                                 enable test mode: gather metrics, print them out, and exit. Note: Test mode only runs inputs, not processors, aggregators, or outputs (default: false)
   --deprecation-list                                     print all deprecated plugins or plugin options (default: false)
   --input-list                                           print available input plugins (default: false)
   --output-list                                          print available output plugins (default: false)
   --version                                              DEPRECATED: display the version and exit (default: false)
   --sample-config                                        DEPRECATED: print out full sample configuration (default: false)
   --plugin-directory value                               DEPRECATED: path to directory containing external plugins
   --section-filter value                                 filter the sections to print, separator is ':'. Valid values are 'agent', 'global_tags', 'outputs', 'processors', 'aggregators' and 'inputs'
   --input-filter value                                   filter the inputs to enable, separator is ':'
   --output-filter value                                  filter the outputs to enable, separator is ':'
   --aggregator-filter value                              filter the aggregators to enable, separator is ':'
   --processor-filter value                               filter the processors to enable, separator is ':'
   --secretstore-filter value                             filter the secret-stores to enable, separator is ':'
   --help, -h                                             show help
```

```
$ ./telegraf --usage cpu

# Read metrics about cpu usage
[[inputs.cpu]]
  ## Whether to report per-cpu stats or not
  percpu = true
  ## Whether to report total system cpu stats or not
  totalcpu = true
  ## If true, collect raw CPU time metrics
  collect_cpu_time = false
  ## If true, compute and report the sum of all non-idle CPU states
  ## NOTE: The resulting 'time_active' field INCLUDES 'iowait'!
  report_active = false
  ## If true and the info is available then add core_id and physical_id tags
  core_tags = false

```

## Config (telegraf.conf)

### agent

``` bash
## Default data collection interval for all inputs
interval = "10s"
## Rounds collection interval to 'interval'
## ie, if interval="10s" then always collect on :00, :10, :20, etc.
round_interval = true
```

interval 选项设置了所有输入插件的默认数据收集间隔。interval = "10s" 表示 Telegraf 将每隔 10 秒从输入插件收集数据一次。可以根据需要调整这个间隔，例如，如果希望更频繁地收集数据，可以将其设置为 "5s"，如果希望减少收集频率，可以将其设置为 "30s" 等。

round_interval 选项决定是否将收集间隔四舍五入到最接近的 interval。如果设置为 true，则 Telegraf 将始终在整数倍的 interval 时间点上收集数据。例如，如果 interval = "10s"，则 Telegraf 将始终在 :00、:10、:20 等时间点上进行数据收集。这有助于保持数据收集的一致性和可预测性。如果设置为 false，则 Telegraf 将在启动后立即开始收集数据，然后每隔 interval 时间收集一次，而不考虑实际的时间点。

在示例中，round_interval = true 表示 Telegraf 将四舍五入收集间隔，确保始终在整数倍的 interval 时间点上收集数据。


``` bash
## Telegraf will send metrics to outputs in batches of at most
## metric_batch_size metrics.
## This controls the size of writes that Telegraf sends to output plugins.
metric_batch_size = 1000
```

metric_batch_size 选项设置了 Telegraf 在将度量值（metrics）发送到输出插件时，每批次发送的度量值的最大数量。这个选项控制了 Telegraf 发送给输出插件的写入大小。

在示例中，metric_batch_size = 1000 表示 Telegraf 将每次最多发送 1000 个度量值给输出插件。这意味着，当 Telegraf 收集到数据后，它将把度量值分成大小为 1000 的批次，并将这些批次依次发送给输出插件。

这个设置可以帮助平衡输出插件的性能与资源占用。较大的批次可能会导致更高的内存占用，但可能会降低输出插件的写入次数。较小的批次可能会减少内存占用，但可能会增加输出插件的写入次数。可以根据输出插件的性能要求和系统资源来调整这个设置。



``` bash
## Maximum number of unwritten metrics per output.  Increasing this value
## allows for longer periods of output downtime without dropping metrics at the
## cost of higher maximum memory usage.
metric_buffer_limit = 10000
```

metric_buffer_limit 用于设置可以在内存中缓存的度量值（metrics）的最大数量。

如果 Telegraf 由于某种原因（例如，输出插件的目标数据库无法接收数据）无法发送数据，那么 Telegraf 会开始在内存中缓存度量值。这个缓存是有限的，metric_buffer_limit 就是用来设置这个缓存的大小的。当缓存达到 metric_buffer_limit 设置的限制时，Telegraf 将停止从输入插件接收新的度量值，直到有足够的空间为止。

metric_buffer_limit = 10000 表示 Telegraf 在内存中最多可以缓存 10000 个度量值。这个设置可以根据系统资源和需求进行调整。


``` bash
## Collection jitter is used to jitter the collection by a random amount.
## Each plugin will sleep for a random time within jitter before collecting.
## This can be used to avoid many plugins querying things like sysfs at the
## same time, which can have a measurable effect on the system.
collection_jitter = "0s"
```

collection_jitter 选项用于在收集度量值之前，为每个输入插件添加一个随机的延迟。这个延迟的时间会在 0 到设置的 collection_jitter 之间随机选择。这样可以避免许多插件在同一时间查询系统资源，如 sysfs，从而降低对系统性能的影响。

在示例中，collection_jitter = "0s" 表示没有为插件添加任何随机延迟。这意味着所有输入插件将在每个收集间隔（由 interval 设置）的开始时同时收集数据。

如果希望降低 Telegraf 对系统性能的影响，可以考虑为 collection_jitter 设置一个较大的值。例如，如果将 collection_jitter 设置为 "5s"，则每个输入插件在每个收集间隔开始时，将等待 0 到 5 秒之间的随机时间，然后再收集数据。这样可以将插件的数据收集操作分散在整个收集间隔内，从而减轻对系统性能的影响。


``` bash
## Collection offset is used to shift the collection by the given amount.
## This can be be used to avoid many plugins querying constraint devices
## at the same time by manually scheduling them in time.
collection_offset = "0s"
```

collection_offset 选项用于将数据收集操作在时间上平移指定的时间量。这可以用于手动安排插件在不同的时间收集数据，从而避免许多插件在同一时间查询受限制的设备。

在示例中，collection_offset = "0s" 表示没有为插件添加任何偏移。这意味着所有输入插件将在每个收集间隔（由 interval 设置）的开始时收集数据。

如果希望手动调整插件的数据收集时间，可以为 collection_offset 设置一个较大的值。例如，如果将 collection_offset 设置为 "5s"，则每个输入插件在每个收集间隔开始时，将等待 5 秒，然后再收集数据。这样可以将插件的数据收集操作在时间上错开，从而避免许多插件在同一时间查询受限制的设备。

请注意，collection_offset 与 collection_jitter 不同。collection_jitter 为每个插件添加一个随机延迟，而 collection_offset 为每个插件添加一个固定的延迟。根据需求可以选择使用这两个选项中的一个或同时使用两者。

``` bash
## Default flushing interval for all outputs. Maximum flush_interval will be
## flush_interval + flush_jitter
flush_interval = "10s"
## Jitter the flush interval by a random amount. This is primarily to avoid
## large write spikes for users running a large number of telegraf instances.
## ie, a jitter of 5s and interval 10s means flushes will happen every 10-15s
flush_jitter = "0s"
```

这些选项涉及 Telegraf 的数据刷新设置：

flush_interval 选项设置了所有输出插件的默认刷新间隔。在示例中，flush_interval = "10s" 表示 Telegraf 将每隔 10 秒将收集到的度量值发送到输出插件一次。可以根据需要调整这个间隔，例如，如果希望更频繁地刷新数据，可以将其设置为 "5s"，如果希望减少刷新频率，可以将其设置为 "30s" 等。

flush_jitter 选项用于在刷新间隔中添加一个随机的时间量。这主要是为了避免在运行大量 Telegraf 实例的用户中产生大的写入峰值。例如，如果 flush_jitter 为 "5s"，且 flush_interval 为 "10s"，则刷新将在每隔 10 到 15 秒之间发生。这可以将 Telegraf 实例的刷新操作在时间上错开，从而降低对输出目标（如数据库）的负载。

在示例中，flush_jitter = "0s" 表示没有添加任何随机延迟，这意味着 Telegraf 将在每个刷新间隔（由 flush_interval 设置）的开始时将数据发送到输出插件。如果希望降低 Telegraf 对输出目标的负载，可以考虑为 flush_jitter 设置一个较大的值。

``` bash
## Collected metrics are rounded to the precision specified. Precision is
## specified as an interval with an integer + unit (e.g. 0s, 10ms, 2us, 4s).
## Valid time units are "ns", "us" (or "µs"), "ms", "s".
##
## By default or when set to "0s", precision will be set to the same
## timestamp order as the collection interval, with the maximum being 1s:
##   ie, when interval = "10s", precision will be "1s"
##       when interval = "250ms", precision will be "1ms"
##
## Precision will NOT be used for service inputs. It is up to each individual
## service input to set the timestamp at the appropriate precision.
precision = "0s"
```

precision 选项用于设置 Telegraf 收集和存储度量值时使用的时间精度。精度以整数 + 单位（例如 "0s"、"10ms"、"2us"、"4s"）的形式指定。有效的时间单位有 "ns"（纳秒）、"us"（微秒，也可以用 "µs" 表示）、"ms"（毫秒）和 "s"（秒）。

默认情况下，或者当设置为 "0s" 时，精度将设置为与收集间隔相同的时间戳顺序，最大值为 1 秒。例如，当 interval = "10s" 时，precision 将为 "1s"；当 interval = "250ms" 时，precision 将为 "1ms"。

对于服务输入，precision 选项将不起作用。每个服务输入需要自行设置适当的时间戳精度。

在示例中，precision = "0s" 表示 Telegraf 将根据收集间隔自动设置时间精度。如果需要更高或更低的时间精度，可以将 precision 设置为所需的值，例如 "1ms"、"10us" 等。


## [InfluxDB line protocol reference](https://docs.influxdata.com/influxdb/v1/write_protocols/line_protocol_reference/)

InfluxDB line protocol is a text-based format for writing points to InfluxDB.

### [Line protocol syntax](https://docs.influxdata.com/influxdb/v1/write_protocols/line_protocol_reference/)

``` xml
<measurement>[,<tag_key>=<tag_value>[,<tag_key>=<tag_value>]] <field_key>=<field_value>[,<field_key>=<field_value>] [<timestamp>]
```

Line protocol accepts the newline character `\n` and is whitespace-sensitive.

> Note: Line protocol does not support the newline character `\n` in tag values or field values.

In InfluxDB's line protocol, the newline character `\n` is used to separate different data points. It is indeed whitespace-sensitive. Whitespace, commas, and equal signs have special meanings and need to be handled properly.

Here is an example of how data is formatted in line protocol:

``` bash
measurementName,tagKey=tagValue fieldKey="fieldValue" timestamp
```

* `measurementName` is the name of the InfluxDB measurement where the data point will be stored.
* `tagKey=tagValue` represents metadata tags associated with the data point.
* `fieldKey="fieldValue"` is the actual data point value.
* `timestamp` is the timestamp for the data point.

Each part is separated by a space, and a newline `\n` is used to separate different data points. If you want to insert whitespace into a field value or tag value, you need to escape it with a backslash `\` .

### Syntax description

InfluxDB line protocol informs InfluxDB of the data's `measurement`, `tag set`, `field set`, and `timestamp`.

| Element | Optional/Required | Description | Type (See [data types](https://docs.influxdata.com/influxdb/v1/write_protocols/line_protocol_reference/#data-types) for more information.)
| -- | -- | -- | --
| [Measurement](https://docs.influxdata.com/influxdb/v1/concepts/glossary/#measurement) | Required | The measurement name. InfluxDB accepts one measurement per point. | String
| [Tag set](https://docs.influxdata.com/influxdb/v1/concepts/glossary/#tag-set) | Optional | All tag key-value pairs for the point. | [Tag keys](https://docs.influxdata.com/influxdb/v1/concepts/glossary/#tag-key) and [tag values](https://docs.influxdata.com/influxdb/v1/concepts/glossary/#tag-value) are both strings.
| [Field set](https://docs.influxdata.com/influxdb/v1/concepts/glossary/#field-set) | Required. Points must have at least one field. | All field key-value pairs for the point. | [Field keys](https://docs.influxdata.com/influxdb/v1/concepts/glossary/#field-key) are strings. [Field values](https://docs.influxdata.com/influxdb/v1/concepts/glossary/#field-value) can be `floats`, `integers`, `strings`, or `Booleans`.
| [Timestamp](https://docs.influxdata.com/influxdb/v1/concepts/glossary/#timestamp) | Optional. InfluxDB uses the server's local nanosecond timestamp in UTC if the timestamp is not included with the point. | The timestamp for the data point. InfluxDB accepts one timestamp per point. | Unix nanosecond timestamp. Specify alternative precisions with the [InfluxDB API](https://docs.influxdata.com/influxdb/v1/tools/api/#write-http-endpoint).

> Performance tips:

1. Before sending data to InfluxDB, sort by tag key to match the results from the [Go bytes.Compare function](http://golang.org/pkg/bytes/#Compare).
2. To significantly improve compression, use the coarsest [precision](https://docs.influxdata.com/influxdb/v1/tools/api/#write-http-endpoint) possible for timestamps.
3. Use the Network Time Protocol (NTP) to synchronize time between hosts. InfluxDB uses a host's local time in UTC to assign timestamps to data. If a host's clock isn't synchronized with NTP, the data that the host writes to InfluxDB may have inaccurate timestamps.

这些建议主要是为了提高 InfluxDB 的性能和数据准确性：

1. "在发送数据到 InfluxDB 之前，按照标签键排序以匹配 Go bytes.Compare 函数的结果"：InfluxDB 存储数据时，会对标签键进行排序，以便在查询时能更快地定位到数据。如果在发送数据到 InfluxDB 之前，就按照标签键进行排序，那么 InfluxDB 在存储数据时就不需要再进行排序，从而提高了性能。
2. "为了显著提高压缩效果，对时间戳使用尽可能粗糙的精度"：InfluxDB 在存储数据时，会对数据进行压缩，以节省存储空间。时间戳的精度越高（例如，精确到纳秒），需要的存储空间就越大，压缩效果就越差。因此，如果不需要高精度的时间戳，使用较低的精度（例如，精确到秒）可以提高压缩效果，从而节省存储空间。
3. "使用网络时间协议（NTP）在主机之间同步时间。InfluxDB 使用主机的本地时间（UTC）为数据分配时间戳。如果主机的时钟没有与 NTP 同步，那么主机写入 InfluxDB 的数据可能会有不准确的时间戳"：这个建议很直接，就是为了确保数据的时间戳准确，需要在主机之间同步时间。如果主机的时钟不准确，那么数据的时间戳也会不准确，这可能会影响到数据的准确性和查询结果。


### Data types

![influxdb_data_type](/assets/images/202401/influxdb_data_type.png)


### Examples

Write the field value `-1.234456e+78` as a **float** to InfluxDB

``` xml
> INSERT mymeas value=-1.234456e+78
```

Write a field value `1.0` as a **float** to InfluxDB

``` xml
> INSERT mymeas value=1.0
```

Write the field value `1` as a **float** to InfluxDB

``` xml
> INSERT mymeas value=1
```

Write the field value `1` as an **integer** to InfluxDB

``` xml
> INSERT mymeas value=1i
```

Write the field value `stringing along` as a **string** to InfluxDB

``` xml
> INSERT mymeas value="stringing along"
```

Write the field value `true` as a **Boolean** to InfluxDB

``` xml
> INSERT mymeas value=true
```

Do not quote Boolean field values. The following statement writes `true` as a **string** field value to InfluxDB:

``` xml
> INSERT mymeas value="true"
```









# Refer

* [InfluxDB frequently asked questions](https://docs.influxdata.com/influxdb/v1/troubleshooting/frequently-asked-questions/#why-can-t-i-query-boolean-field-values)












