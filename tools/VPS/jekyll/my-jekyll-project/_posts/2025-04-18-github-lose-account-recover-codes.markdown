---
layout: post
title:  "GitHub 使用 SMS 登录失败且丢失 github-recovery-codes 的账户恢复方法"
date:   2025-04-18 12:30:00 +0800
categories: 版本控制
---

* Do not remove this line (it will not be displayed)
{:toc}

# 问题背景

在中国大陆登陆 GitHub 账号使用 SMS 短信验证出现无法收到验证码的情况。GitHub 给出的解决办法是：If you are unable to access your mobile device, enter one of your recovery codes to verify your identity. 即，如果你不能使用 SMS 短信验证，可以使用 recovery codes 进行验证。

![github1](/assets/images/202504/github1.png)

![github2](/assets/images/202504/github2.png)

![github3](/assets/images/202504/github3.png)

那么问题是：[Where to find recovery codes](https://docs.github.com/en/authentication/securing-your-account-with-two-factor-authentication-2fa/recovering-your-account-if-you-lose-your-2fa-credentials#using-a-two-factor-authentication-recovery-code)

官方对于 Using a two-factor authentication recovery code 的相关介绍：

> Use one of your recovery codes to automatically regain entry into your account. You may have saved your recovery codes to a password manager or your computer's downloads folder. The default filename for recovery codes is github-recovery-codes.txt. For more information about recovery codes, see [Configuring two-factor authentication recovery methods](https://docs.github.com/en/authentication/securing-your-account-with-two-factor-authentication-2fa/configuring-two-factor-authentication-recovery-methods#downloading-your-two-factor-authentication-recovery-codes).

官方建议用户将 recovery codes 进行安全保存，这个信息默认会存储在 `github-recovery-codes.txt` 文件中。那么如何获取这个文件呢？

参考 [Downloading your two-factor authentication recovery codes](https://docs.github.com/en/authentication/securing-your-account-with-two-factor-authentication-2fa/configuring-two-factor-authentication-recovery-methods#downloading-your-two-factor-authentication-recovery-codes) 的介绍。可以获知，当你配置了 `two-factor authentication` 后，你可以下载和保存 `2FA recovery codes`。这个 recovery codes 可以用来作为登陆 GitHub 账户的鉴权凭证。**问题是，什么时候开启的 `two-factor authentication` 认证方式**。

> When you configure two-factor authentication, you'll download and save your 2FA recovery codes. If you lose access to your phone, you can authenticate to GitHub using your recovery codes. You can also download your recovery codes at any point after enabling two-factor authentication.
>
> To keep your account secure, don't share or distribute your recovery codes. We recommend saving them with a secure password manager.
>
> If you generate new recovery codes or disable and re-enable 2FA, the recovery codes in your security settings automatically update. Reconfiguring your 2FA settings without disabling 2FA will not change your recovery codes.

下面方法说明了如何保存 `github-recovery-codes.txt` 信息：

![github4](/assets/images/202504/github4.png)

**官方同时也提示说，如果一旦你使用过 recovery code 去登陆账户，这个 code 将不能被再次使用，需要重新生成新的 code。** 申请新的 code 的方法如下：

![github5](/assets/images/202504/github5.png)

# 解决方法

**目前遇到的问题是，知道 GitHub 账号的登陆密码，但是 SMS 短信验证方式使用异常，并且 recovery code 之前也没有保存，此时如何恢复账户登陆？**于是向 GitHub 官方发送了一封问题咨询邮件，并得到了官方的回复。

问题咨询邮件的内容：

![github6](/assets/images/202504/github6.png)


GitHub 官方回复的内容：

![github7](/assets/images/202504/github7.png)

![github8](/assets/images/202504/github8.png)

关键信息如下：

* 首先官方回复比较慢，大概一周多时间。
* 官方建议最快的恢复方法是使用 account recovery codes，并给出了指引：[Using a two-factor authentication recovery code](https://docs.github.com/authentication/securing-your-account-with-two-factor-authentication-2fa/recovering-your-account-if-you-lose-your-2fa-credentials#using-a-two-factor-authentication-recovery-code)
* 官方也解释了 `SMS Troubleshooting Advice` 的问题和解决方法，主要是由 SMS provider 提供的能力决定的。
* 比较有用的信息是，官方另外提供了 `Account Recovery Flow` 的指引。如果当前有 SSH key 或者已经认证过的设备，那么用户可以提交一个 recovery request 看能否 regain access。

> Do you have access to **an SSH key** or **a verified device** associated with the account? **If so, you could submit a recovery request to potentially regain access. To start an account recovery request**, begin by reading the helpful documentation below: [Authenticating with a verified device, SSH key, or personal access token](https://docs.github.com/github/authenticating-to-github/securing-your-account-with-two-factor-authentication-2fa/recovering-your-account-if-you-lose-your-2fa-credentials#authenticating-with-a-verified-device-ssh-token-or-personal-access-token)

关于 [Authenticating with a verified device, SSH token, or personal access token](https://docs.github.com/en/authentication/securing-your-account-with-two-factor-authentication-2fa/recovering-your-account-if-you-lose-your-2fa-credentials#authenticating-with-a-verified-device-ssh-token-or-personal-access-token) 的相关说明：

> If you know your password for GitHub but don't have the two-factor authentication credentials or your two-factor authentication recovery codes, you can have a one-time password sent to your verified email address to begin the verification process. You'll need to verify your identity using a recovery authentication factor, such as an SSH key or previously verified device.

可以看到 `an SSH key or previously verified device` 成为了最后的救命稻草。


![github9](/assets/images/202504/github9.png)


Authenticating with a verified device, SSH token, or personal access token 的具体申请方法如下：

![github12](/assets/images/202504/github12.png)

操作步骤：

1. 第一步，点击 **I understand, get started** 后会收到 GitHub 官方的邮件 one-time password (OTP)，完成第一步认证。
2. 第二步，Choose a recovery verification factor，由于能找到可以提交 GitHub 仓库的工程环境，因此选择 SSH key 的验证方式。
3. 最后，GitHub Support 将会 review 这个申请在三天左右。如果这个申请审批通过，就会收到一个链接恢复账户登陆。

![github13](/assets/images/202504/github13.png)

![github10](/assets/images/202504/github10.png)

![github11](/assets/images/202504/github11.png)

![github14](/assets/images/202504/github14.png)


# GitHub Support 的回复

![github15](/assets/images/202504/github15.png)


# Refer

* https://docs.github.com/en/authentication/troubleshooting-ssh/error-permission-denied-publickey
* [Recovering your account if you lose your 2FA credentials](https://docs.github.com/en/authentication/securing-your-account-with-two-factor-authentication-2fa/recovering-your-account-if-you-lose-your-2fa-credentials)

