---
layout: post
title:  "Kernel TLS in Action"
date:   2025-05-25 20:00:00 +0800
categories: [Linux Performance]
---

* Do not remove this line (it will not be displayed)
{:toc}


# Overview

**Transport Layer Security** (`TLS`) is a Upper Layer Protocol (ULP) that runs over TCP. TLS provides end-to-end data integrity and confidentiality.

# User interface

## Creating a TLS connection

First create a new TCP socket and set the TLS ULP.

``` c
sock = socket(AF_INET, SOCK_STREAM, 0);
setsockopt(sock, SOL_TCP, TCP_ULP, "tls", sizeof("tls"));
```

Setting the TLS ULP allows us to set/get TLS socket options. Currently only the symmetric encryption is handled in the kernel. After the TLS handshake is complete, we have all the parameters required to move the data-path to the kernel. There is a separate socket option for moving the transmit and the receive into the kernel.

``` c
/* From linux/tls.h */
struct tls_crypto_info {
        unsigned short version;
        unsigned short cipher_type;
};

struct tls12_crypto_info_aes_gcm_128 {
        struct tls_crypto_info info;
        unsigned char iv[TLS_CIPHER_AES_GCM_128_IV_SIZE];
        unsigned char key[TLS_CIPHER_AES_GCM_128_KEY_SIZE];
        unsigned char salt[TLS_CIPHER_AES_GCM_128_SALT_SIZE];
        unsigned char rec_seq[TLS_CIPHER_AES_GCM_128_REC_SEQ_SIZE];
};


struct tls12_crypto_info_aes_gcm_128 crypto_info;

crypto_info.info.version = TLS_1_2_VERSION;
crypto_info.info.cipher_type = TLS_CIPHER_AES_GCM_128;
memcpy(crypto_info.iv, iv_write, TLS_CIPHER_AES_GCM_128_IV_SIZE);
memcpy(crypto_info.rec_seq, seq_number_write,
                                      TLS_CIPHER_AES_GCM_128_REC_SEQ_SIZE);
memcpy(crypto_info.key, cipher_key_write, TLS_CIPHER_AES_GCM_128_KEY_SIZE);
memcpy(crypto_info.salt, implicit_iv_write, TLS_CIPHER_AES_GCM_128_SALT_SIZE);

setsockopt(sock, SOL_TLS, TLS_TX, &crypto_info, sizeof(crypto_info));
```

Transmit and receive are set separately, but the setup is the same, using either `TLS_TX` or `TLS_RX`.


## Sending TLS application data

After setting the `TLS_TX` socket option all application data sent over this socket is encrypted using TLS and the parameters provided in the socket option. For example, we can send an encrypted `hello world` record as follows:

``` c
const char *msg = "hello world\n";
send(sock, msg, strlen(msg));
```

`send()` data is directly encrypted from the userspace buffer provided to the encrypted kernel send buffer if possible.

The `sendfile` system call will send the file’s data over TLS records of maximum length (`2^14`).

``` c
file = open(filename, O_RDONLY);
fstat(file, &stat);
sendfile(sock, file, &offset, stat.st_size);
```

TLS records are created and sent after each `send()` call, unless `MSG_MORE` is passed. `MSG_MORE` will delay creation of a record until `MSG_MORE` is not passed, or the maximum record size is reached.

The kernel will need to allocate a buffer for the encrypted data. This buffer is allocated at the time `send()` is called, such that either the entire `send()` call will return `-ENOMEM` (or block waiting for memory), or the encryption will always succeed. If `send()` returns `-ENOMEM` and some data was left on the socket buffer from a previous call using `MSG_MORE`, the `MSG_MORE` data is left on the socket buffer.


## Receiving TLS application data

After setting the `TLS_RX` socket option, all recv family socket calls are decrypted using TLS parameters provided. A full TLS record must be received before decryption can happen.

``` c
char buffer[16384];
recv(sock, buffer, 16384);
```

Received data is decrypted directly in to the user buffer if it is large enough, and no additional allocations occur. If the userspace buffer is too small, data is decrypted in the kernel and copied to userspace.

* **EINVAL** is returned if the TLS version in the received message does not match the version passed in setsockopt.
* **EMSGSIZE** is returned if the received message is too big.
* **EBADMSG** is returned if decryption failed for any other reason.


## Send TLS control messages

Other than application data, TLS has control messages such as **alert messages** (record type 21) and **handshake messages** (record type 22), etc. These messages can be sent over the socket by providing the TLS record type via a `CMSG`. For example the following function sends `@data` of `@length` bytes using a record of type `@record_type`.

``` c
/* send TLS control message using record_type */
static int klts_send_ctrl_message(int sock, unsigned char record_type,
                                  void *data, size_t length)
{
      struct msghdr msg = {0};
      int cmsg_len = sizeof(record_type);
      struct cmsghdr *cmsg;
      char buf[CMSG_SPACE(cmsg_len)];
      struct iovec msg_iov;   /* Vector of data to send/receive into.  */

      msg.msg_control = buf;
      msg.msg_controllen = sizeof(buf);
      cmsg = CMSG_FIRSTHDR(&msg);
      cmsg->cmsg_level = SOL_TLS;
      cmsg->cmsg_type = TLS_SET_RECORD_TYPE;
      cmsg->cmsg_len = CMSG_LEN(cmsg_len);
      *CMSG_DATA(cmsg) = record_type;
      msg.msg_controllen = cmsg->cmsg_len;

      msg_iov.iov_base = data;
      msg_iov.iov_len = length;
      msg.msg_iov = &msg_iov;
      msg.msg_iovlen = 1;

      return sendmsg(sock, &msg, 0);
}
```

Control message data should be provided unencrypted, and will be encrypted by the kernel.

## Receiving TLS control messages

TLS control messages are passed in the userspace buffer, with `message type` passed via `cmsg`. If no `cmsg` buffer is provided, an error is returned if a control message is received. Data messages may be received without a `cmsg` buffer set.

``` c
char buffer[16384];
char cmsg[CMSG_SPACE(sizeof(unsigned char))];
struct msghdr msg = {0};
msg.msg_control = cmsg;
msg.msg_controllen = sizeof(cmsg);

struct iovec msg_iov;
msg_iov.iov_base = buffer;
msg_iov.iov_len = 16384;

msg.msg_iov = &msg_iov;
msg.msg_iovlen = 1;

int ret = recvmsg(sock, &msg, 0 /* flags */);

struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
if (cmsg->cmsg_level == SOL_TLS &&
    cmsg->cmsg_type == TLS_GET_RECORD_TYPE) {
    int record_type = *((unsigned char *)CMSG_DATA(cmsg));
    // Do something with record_type, and control message data in
    // buffer.
    //
    // Note that record_type may be == to application data (23).
} else {
    // Buffer contains application data.
}
```

`recv` will never return data from mixed types of TLS records.


# TLS 1.3 Key Updates

In TLS 1.3, KeyUpdate handshake messages signal that the sender is updating its TX key. Any message sent after a KeyUpdate will be encrypted using the new key. The userspace library can pass the new key to the kernel using the `TLS_TX` and `TLS_RX` socket options, as for the initial keys. TLS version and cipher cannot be changed.

To prevent attempting to decrypt incoming records using the wrong key, decryption will be paused when a KeyUpdate message is received by the kernel, until the new key has been provided using the `TLS_RX` socket option. Any read occurring after the KeyUpdate has been read and before the new key is provided will fail with `EKEYEXPIRED`. `poll()` will not report any read events from the socket until the new key is provided. There is no pausing on the transmit side.

Userspace should make sure that the `crypto_info` provided has been set properly. In particular, the kernel will not check for key/nonce reuse.

The number of successful and failed key updates is tracked in the `TlsTxRekeyOk`, `TlsRxRekeyOk`, `TlsTxRekeyError`, `TlsRxRekeyError` statistics. The `TlsRxRekeyReceived` statistic counts KeyUpdate handshake messages that have been received.


# Integrating in to userspace TLS library

**At a high level, the kernel TLS ULP is a replacement for the record layer of a userspace TLS library**.

A patchset to OpenSSL to use ktls as the record layer is [here](https://github.com/Mellanox/openssl/commits/tls_rx2).

[An example](https://github.com/ktls/af_ktls-tool/commits/RX) of calling send directly after a handshake using gnutls. Since it doesn’t implement a full record layer, control messages are not supported.


# Statistics

TLS implementation exposes the following per-namespace statistics (`/proc/net/tls_stat`):

* `TlsCurrTxSw`, `TlsCurrRxSw` - number of TX and RX sessions currently installed where host handles cryptography
* `TlsCurrTxDevice`, `TlsCurrRxDevice` - number of TX and RX sessions currently installed where NIC handles cryptography
* `TlsTxSw`, `TlsRxSw` - number of TX and RX sessions opened with host cryptography
* `TlsTxDevice`, `TlsRxDevice` - number of TX and RX sessions opened with NIC cryptography
* `TlsDecryptError` - record decryption failed (e.g. due to incorrect authentication tag)
* `TlsDeviceRxResync` - number of RX resyncs sent to NICs handling cryptography
* `TlsDecryptRetry` - number of RX records which had to be re-decrypted due to TLS_RX_EXPECT_NO_PAD mis-prediction. Note that this counter will also increment for non-data records.
* `TlsRxNoPadViolation` - number of data RX records which had to be re-decrypted due to TLS_RX_EXPECT_NO_PAD mis-prediction.
* `TlsTxRekeyOk`, `TlsRxRekeyOk` - number of successful rekeys on existing sessions for TX and RX
* `TlsTxRekeyError`, `TlsRxRekeyError` - number of failed rekeys on existing sessions for TX and RX
* `TlsRxRekeyReceived` - number of received KeyUpdate handshake messages, requiring userspace to provide a new RX key


# [Improving NGINX Performance with Kernel TLS and SSL_sendfile](https://www.f5.com/company/blog/nginx/improving-nginx-performance-with-kernel-tls)

**Transport Layer Security** (`TLS`) is an extremely popular cryptography protocol. **Implementing `TLS` in the kernel (`kTLS`) improves performance by significantly reducing the need for copying operations between user space and the kernel**.

**Combining `kTLS` and `sendfile()` means data is encrypted directly in kernel space, before being passed to the network stack for transmission. This eliminates the need to copy data into user space to be encrypted by TLS libraries and then back into kernel space for transmission. `kTLS` also enables offload of TLS processing to hardware, including [offload of TLS symmetric crypto processing to network devices](https://people.freebsd.org/~gallatin/talks/euro2019-ktls.pdf).**

**Modern Linux and FreeBSD kernels support offloading TLS to the kernel**, and now NGINX Open Source does too! [NGINX 1.21.4 introduces support for kTLS](https://hg.nginx.org/nginx/rev/65946a191197) when serving static files and cached responses with [SSL_sendfile()](https://www.openssl.org/docs/manmaster/man3/SSL_sendfile.html), which can hugely improve performance. As detailed below, both the kernel and OpenSSL must be built with kTLS for NGINX to use `SSL_sendfile()`.

In this blog we detail which operating system and OpenSSL versions support kTLS, and show how to build and configure the kernel and NGINX for kTLS. To give you an idea of the performance improvement you can expect from kTLS, we also share the specs and results of our testing on FreeBSD and Ubuntu.

> **Note**: kTLS implementations are quite new and evolving rapidly. This blog describes support for kTLS as of November 2021, but keep an eye out for announcements on [nginx.org](https://nginx.org/en/).


## General Requirements

* Operating system – Either of:
    + FreeBSD 13.0+. As of November 2021, FreeBSD 13.0+ is the only OS that supports `kTLS` in NGINX without a manual build of NGINX to incorporate OpenSSL 3.0.0+. See [Enabling NGINX with kTLS on FreeBSD](https://www.f5.com/company/blog/nginx/improving-nginx-performance-with-kernel-tls#nginx-ktls-freebsd).
    + **A Linux distribution built on Linux kernel version 4.17 or later, though we recommend using those built on version 5.2 or later when possible**. (`kTLS` support is actually available in version 4.13, but OpenSSL 3.0.0 requires kernel header version 4.17 or later.)

* OpenSSL – Version 3.0.0 or later

* NGINX – Version 1.21.4 or later (mainline)


## Enabling kTLS in NGINX

As mentioned in the introduction, **`kTLS` improves NGINX performance because all encryption and decryption take place in the kernel. Data is encrypted directly in kernel space – before being passed to the network stack for transmission – eliminating the need to copy data into user space to be encrypted by TLS libraries and then back into kernel space for transmission**.

![kTLS-NGINX_topology](/assets/images/202505/kTLS-NGINX_topology.png)


## Loading kTLS in the Kernel

In modern FreeBSD and Linux distributions, **`kTLS` is usually built as a module** (with the `CONFIG_TLS=m` option). **You must explicitly load the `kTLS` module into the kernel before you start NGINX**.

* On FreeBSD, run these commands as the **root** user:

``` bash
# kldload ktls_ocf.ko
# sysctl kern.ipc.tls.enable=1
```

For details about the FreeBSD command options, see the man page for [ktls(4)](https://www.freebsd.org/cgi/man.cgi?query=ktls&apropos=0&sektion=0&manpath=FreeBSD+13.0-RELEASE+and+Ports&arch=default&format=html).


* On Linux distributions, run this command as the **root** user:

``` bash
# modprobe tls
```

## Configuring NGINX

To enable kTLS, include the [ssl_conf_command](https://nginx.org/en/docs/http/ngx_http_ssl_module.html#ssl_conf_command) directive with the `Options KTLS` parameter in the `server{}` context, as in this sample configuration used for our [testing](https://www.f5.com/company/blog/nginx/improving-nginx-performance-with-kernel-tls#testing):

```
worker_processes auto;error_log /var/log/nginx/error.log debug;

events {}

http {
    sendfile on;

    server {
        listen 443 ssl;
        ssl_certificate ssl/example.crt;
        ssl_certificate_key ssl/example.key;
        ssl_conf_command Options KTLS;
        ssl_protocols TLSv1.3;

        location / {
            root /data;
    	}
    }
}
```


## Verifying kTLS is Enabled

To verify that NGINX is using `kTLS`, [enable debugging mode](https://docs.nginx.com/nginx/admin-guide/monitoring/debugging/) and check for `BIO_get_ktls_send()` and `SSL_sendfile()` in the error log.

![kTLS-NGINX](/assets/images/202505/kTLS-NGINX.png)

> **Note**: We recommend that you disable debugging mode after making these checks, especially in production environments. Debug logging incurs a performance penalty due to the large volume of write operations; also, debug logs can be huge and quickly exhaust available space on the disk partition.


## Performance Improvement with kTLS

When serving static files and cached responses under heavy load, `SSL_sendfile()` can [increase throughput by up to 2x](https://people.freebsd.org/~gallatin/talks/euro2019-ktls.pdf) compared to user‑space TLS, but the size of the performance boost depends significantly on various factors (disk performance, system load, etc). It is also possible to reduce CPU usage if your network card supports TLS offload.

### Testing Performance

**To measure the performance boost on your setup, use the following instructions to run a simple one‑thread test. As detailed below, our test results indicate a performance boost of up to nearly 30% without any specific tuning**.

![kTLS-NGINX2](/assets/images/202505/kTLS-NGINX2.png)

To perform the test:

* Create a large file that fits completely in the disk cache:

``` bash
truncate -s 1g /data/1G
```

* Run this command to check the throughput; the base command is repeated multiple times for more accurate results. Pipe the output to the `ministat` utility [FreeBSD](https://www.freebsd.org/cgi/man.cgi?query=ministat) [Ubuntu](https://manpages.ubuntu.com/manpages/impish/man1/ministat.1.html) for a basic statistical analysis.

``` bash
for i in 'seq 1 100'; do curl -k -s -o /dev/null -w '%{speed_download}\n' https://localhost/1G | ministat
```

### Results of Performance Testing

In the following results from our tests, presented as output from `ministat`, each value is the download speed in `kBytes/second`. The output is split across two lines for legibility.

Throughput for FreeBSD 13.0 **without kTLS**:

![kTLS-NGINX3](/assets/images/202505/kTLS-NGINX3.png)

Throughput for FreeBSD 13.0 **with kTLS**:

![kTLS-NGINX4](/assets/images/202505/kTLS-NGINX4.png)

Throughput for Ubuntu 21.10 **without kTLS**:

![kTLS-NGINX5](/assets/images/202505/kTLS-NGINX5.png)

Throughput for Ubuntu 21.10 **with kTLS**:

![kTLS-NGINX6](/assets/images/202505/kTLS-NGINX6.png)


## Summary

NGINX 1.21.4 introduces support for `kTLS` when serving static files and cached responses with `SSL_sendfile()`. **Our testing shows that performance improves by between 8% and 29%, depending on the operating system**.







# Refer

* https://docs.kernel.org/networking/tls.html
* [Improving NGINX Performance with Kernel TLS and SSL_sendfile](https://www.f5.com/company/blog/nginx/improving-nginx-performance-with-kernel-tls)
* https://github.com/torvalds/linux/blob/master/tools/testing/selftests/net/tls.c




