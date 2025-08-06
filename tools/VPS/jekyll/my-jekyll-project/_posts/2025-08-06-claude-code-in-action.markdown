---
layout: post
title:  "Claude Code in Action"
date:   2025-08-06 12:30:00 +0800
categories: ML
---

* Do not remove this line (it will not be displayed)
{:toc}


# Introduction

**Claude Code** is an **agentic coding tool** that lives in your **terminal**, understands your codebase, and helps you code faster by executing routine tasks, explaining complex code, and handling git workflows -- **all through natural language commands**. Use it in your **terminal**, **IDE**, or `tag @claude` on Github.

Learn more in the [official documentation](https://docs.anthropic.com/en/docs/claude-code/overview).


## What Claude Code does for you

* **Build features from descriptions**: Tell Claude what you want to build **in plain English**. It will make a plan, write the code, and ensure it works.

* **Debug and fix issues**: Describe a bug or paste an error message. Claude Code will analyze your codebase, identify the problem, and implement a fix.

* **Navigate any codebase**: Ask anything about your team’s codebase, and get a thoughtful answer back. Claude Code maintains awareness of your entire project structure, can find up-to-date information from the web, and with [MCP](https://docs.anthropic.com/en/docs/claude-code/mcp) can pull from external datasources like `Google Drive`, `Figma`, and `Slack`.

* **Automate tedious tasks**: Fix fiddly lint issues, resolve merge conflicts, and write release notes. Do all this in a single command from your developer machines, or automatically in CI.

## Why developers love Claude Code

* **Works in your terminal**: `Not another chat window. Not another IDE`. Claude Code meets you where you already work, with the tools you already love.

* **Takes action**: Claude Code can directly edit files, run commands, and create commits. Need more? **MCP** lets Claude read your design docs in Google Drive, update your tickets in Jira, or use your custom developer tooling.

* **Unix philosophy**: Claude Code is composable and scriptable.

`tail -f app.log | claude -p "Slack me if you see any anomalies appear in this log stream"` works.

Your CI can run `claude -p "If there are new text strings, translate them into French and raise a PR for @lang-fr-team to review"`.

* **Enterprise-ready**: Use Anthropic’s API, or host on `AWS` or `GCP`. Enterprise-grade security, privacy, and compliance is built-in.




# Prepare

**Prerequisites**: [Node.js 18 or newer](https://nodejs.org/en/download/)

``` bash
# Install Claude Code
npm install -g @anthropic-ai/claude-code

# Navigate to your project
cd your-awesome-project

# Start coding with Claude
claude
```

Got specific setup needs or hit issues? See [advanced setup](https://docs.anthropic.com/en/docs/claude-code/setup) or [troubleshooting](https://docs.anthropic.com/en/docs/claude-code/troubleshooting).



## Install nvm

refer: [Installing nodejs and npm on linux](https://stackoverflow.com/questions/39981828/installing-nodejs-and-npm-on-linux)

I really recommend you install node and npm using [nvm](https://github.com/creationix/nvm) (**Node Version Manager**). This is the fastest, cleanest and easiest way to do it.

To install or update **nvm**, you should run the [install script](https://github.com/nvm-sh/nvm/blob/v0.40.3/install.sh). To do that, you may either download and run the script manually, or use the following `cURL` or `Wget` command:

``` bash
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.3/install.sh | bash

wget -qO- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.3/install.sh | bash
```

Running either of the above commands downloads a script and runs it. The script clones the **nvm** repository to `~/.nvm`, and attempts to add the source lines from the snippet below to the correct profile file (`~/.bashrc`, `~/.bash_profile`, `~/.zshrc`, or `~/.profile`). If you find the install script is updating the wrong profile file, set the `$PROFILE` env var to the profile file’s path, and then rerun the installation script.

``` bash
export NVM_DIR="$([ -z "${XDG_CONFIG_HOME-}" ] && printf %s "${HOME}/.nvm" || printf %s "${XDG_CONFIG_HOME}/nvm")"
[ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh" # This loads nvm
```

最后执行 `. .bashrc` 使得 nvm 环境设置生效。

```
$ nvm -h

Node Version Manager (v0.40.3)

Note: <version> refers to any version-like string nvm understands. This includes:
  - full or partial version numbers, starting with an optional "v" (0.10, v0.1.2, v1)
  - default (built-in) aliases: node, stable, unstable, iojs, system
  - custom aliases you define with `nvm alias foo`

 Any options that produce colorized output should respect the `--no-colors` option.

Usage:
  nvm --help                                  Show this message

...
```

And you are now able to **install node typing**:

``` bash
nvm install <version>
```

For example

``` bash
nvm install 4.2.1
```

If you just want to install the latest node version, you can just type

``` bash
nvm install node
```

![nvm](/assets/images/202508/nvm.png)



## Install Claude Code


``` bash
npm install -g @anthropic-ai/claude-code
```

![npm](/assets/images/202508/npm.png)


# Get started

* https://docs.anthropic.com/en/docs/claude-code/quickstart
* [Continue with Quickstart (5 mins)](https://docs.anthropic.com/en/docs/claude-code/quickstart)





# Q&A

![claude1](/assets/images/202508/claude1.png)






# Refer

* https://www.anthropic.com/claude-code
* https://github.com/anthropics/claude-code
* https://docs.anthropic.com/en/docs/claude-code/overview