---
layout: post
title:  "My Developer Tools"
date:   2023-12-14 11:00:00 +0800
categories: Tools
---

* Do not remove this line (it will not be displayed)
{:toc}

# 编译工具

## CMake 3.20.0

``` bash
#!/bin/bash

CUR_DIR=$(dirname $(readlink -f $0))

CMAKE_DATA_DIR=cmake-3.20.0-linux-x86_64
CMAKE_INSTALL_DIR_PREFIX=/usr/local/bin
export CMAKE_INSTALL_DIR="$CMAKE_INSTALL_DIR_PREFIX/$CMAKE_DATA_DIR"
export PATH="$CMAKE_INSTALL_DIR/bin":$PATH

# Specify the cmake version to install
CMAKE_VERSION="3.20.0"
CMAKE_DOWNLOAD_URL="https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-3.20.0-linux-x86_64.sh"
LOCAL_CMAKE_DIR="$CUR_DIR/$CMAKE_DATA_DIR"

function InstallCMake()
{
        echo "Installing cmake version $CMAKE_VERSION..."

        if [[ -d $LOCAL_CMAKE_DIR ]]; then
                echo "Using local cmake dir: $LOCAL_CMAKE_DIR"
                cp -r $LOCAL_CMAKE_DIR "$CMAKE_INSTALL_DIR_PREFIX/$CMAKE_DATA_DIR"

        elif wget -q -O "cmake-linux.sh" --tries=1 "$CMAKE_DOWNLOAD_URL"; then
                echo "Downloaded cmake from $CMAKE_DOWNLOAD_URL"

                sh $CUR_DIR/cmake-linux.sh -- --skip-license --prefix=$CMAKE_INSTALL_DIR_PREFIX
                rm $CUR_DIR/cmake-linux.sh

        else
                echo "Error: Could not download cmake"
                exit 1
        fi

        echo "cmake installation completed, version:"
        echo "-----------------------------------------"
        echo "$(cmake --version)"
        echo "-----------------------------------------"
}

# Check if the user has root privileges
if [ "$(id -u)" != "0" ]; then
        echo "This script must be run as root" 1>&2
        exit 1
fi

# Check if cmake is already installed
if command -v cmake &> /dev/null; then
        echo "cmake is already installed, $(cmake --version | head -n1)"

        CUR_CMAKE_VERSION=`cmake --version | head -n1 | awk '{print $3}' | awk -F'.' '{printf $1$2}'`
        if [[ "$CUR_CMAKE_VERSION" -lt 320 ]]; then
                echo "cmake3.20 is required, please use $CUR_DIR/tools/cmake-build/cmake-install/install_cmake.sh to upgrade cmake firstly"

                InstallCMake
        fi
else
        InstallCMake

fi
```

# Bazel (Bazelisk 1.17.0)

``` bash
#!/bin/bash

CUR_DIR=$(dirname $(readlink -f $0))

# Specify the Bazelisk version to install
BAZELISK_VERSION="1.17.0"
BAZELISK_DOWNLOAD_URL="https://github.com/bazelbuild/bazelisk/releases/download/v${BAZELISK_VERSION}/bazelisk-linux-amd64"
LOCAL_BAZELISK_FILE="$CUR_DIR/bazelisk-linux-amd64-1.17.0/bazelisk-linux-amd64"

# Check if the user has root privileges
if [ "$(id -u)" != "0" ]; then
    echo "This script must be run as root" 1>&2
    exit 1
fi

# Check if Bazelisk is already installed
if command -v bazel &> /dev/null; then
    echo "Bazelisk is already installed, version: $(bazel --version)"
else
    echo "Installing Bazelisk version ${BAZELISK_VERSION}..."

    if [[ -f "${LOCAL_BAZELISK_FILE}" ]]; then
        echo "Using local Bazelisk file: ${LOCAL_BAZELISK_FILE}"
        cp "${LOCAL_BAZELISK_FILE}" "bazelisk-linux-amd64"

    elif wget -q -O "bazelisk-linux-amd64" --tries=1 "${BAZELISK_DOWNLOAD_URL}"; then
        echo "Downloaded Bazelisk from ${BAZELISK_DOWNLOAD_URL}"

    else
        echo "Error: Could not download Bazelisk and local file not found"
        exit 1
    fi

    # Move Bazelisk to the /usr/local/bin directory and make it executable
    sudo mv "bazelisk-linux-amd64" /usr/local/bin/bazel
    sudo chmod +x /usr/local/bin/bazel

    echo "Bazelisk installation completed, version:"
    echo "-----------------------------------------"
    echo "$(bazel version)"
    echo "-----------------------------------------"
fi
```


## Clang

编译安装 Clang 11.0.0 并指定编译安装一些辅助工具，包括 lld, lldb, clang-tools-extra 等。安装脚本参考如下：

``` bash
#!/bin/bash

# Download the LLVM project
if ! wget https://github.com/llvm/llvm-project/releases/download/llvmorg-11.0.0/llvm-project-11.0.0.tar.xz; then
        echo "Error: Failed to download LLVM project"
        exit 1
fi

# Extract the archive
if ! tar xvfJ llvm-project-11.0.0.tar.xz; then
        echo "Error: Failed to extract the tarball"
        exit 1
fi

# Enter the extracted directory
cd llvm-project-11.0.0 || { echo "Error: Failed to enter the extracted directory"; exit 1; }

# Create a build directory
mkdir build
cd build || { echo "Error: Failed to enter the build directory"; exit 1; }

# Configure the build with CMake
if ! cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_RTTI=ON -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;libcxx;libcxxabi;libunwind;lldb;compiler-rt;lld;polly" -G "Unix Makefiles" ../llvm; then
        echo "Error: Failed to configure the build with CMake"
        exit 1
fi

# Get the number of CPU cores
num_cores=$(nproc)

# Build Clang and other tools
if ! make -j"${num_cores}"; then
        echo "Error: Failed to build Clang and other tools"
        exit 1
fi

# Check if the user has root privileges before installing
if [ "$(id -u)" != "0" ]; then
        echo "Error: Installation requires root privileges" 1>&2
        exit 1
fi

# Install Clang and other tools
if ! make install; then
        echo "Error: Failed to install Clang and other tools"
        exit 1
fi

# Print the installed Clang version
clang --version
```

## GoLang 1.20.7

``` bash
#!/bin/bash

# Download Go
if ! wget https://go.dev/dl/go1.20.7.linux-amd64.tar.gz; then
  echo "Error: Failed to download Go"
  exit 1
fi

# Extract the archive
if ! tar xvf go1.20.7.linux-amd64.tar.gz; then
  echo "Error: Failed to extract the tarball"
  exit 1
fi

# Check if the user has root privileges before installing
if [ "$(id -u)" != "0" ]; then
  echo "Error: Installation requires root privileges" 1>&2
  exit 1
fi

# Move the extracted directory to /usr/local
if ! mv go /usr/local; then
  echo "Error: Failed to move the extracted directory to /usr/local"
  exit 1
fi
```

## Python 3.8.12

``` bash
#!/bin/bash

# Download Python
if ! wget https://www.python.org/ftp/python/3.8.12/Python-3.8.12.tgz; then
  echo "Error: Failed to download Python"
  exit 1
fi

# Extract the archive
if ! tar xvf Python-3.8.12.tgz; then
  echo "Error: Failed to extract the tarball"
  exit 1
fi

# Enter the extracted directory
cd Python-3.8.12 || { echo "Error: Failed to enter the extracted directory"; exit 1; }

# Configure the build
if ! ./configure; then
  echo "Error: Failed to configure the build"
  exit 1
fi

# Get the number of CPU cores
num_cores=$(nproc)

# Build Python
if ! make -j"${num_cores}"; then
  echo "Error: Failed to build Python"
  exit 1
fi

# Check if the user has root privileges before installing
if [ "$(id -u)" != "0" ]; then
  echo "Error: Installation requires root privileges" 1>&2
  exit 1
fi

# Install Python
if ! make install; then
  echo "Error: Failed to install Python"
  exit 1
fi

# Create a soft link for Python
if ! ln -sf /usr/local/bin/python3.8 /bin/python; then
  echo "Error: Failed to create a soft link for Python"
  exit 1
fi

# Print the installed Python version
python --version
```


## CCache

``` bash
yum install ccache -y
```

# Git

## git-lfs

``` bash
#!/bin/bash

# Download Git LFS v3.4.0
if ! wget https://github.com/git-lfs/git-lfs/releases/download/v3.4.0/git-lfs-linux-amd64-v3.4.0.tar.gz; then
        echo "Error: Failed to download Git LFS"
        exit 1
fi

# Extract the archive
if ! tar xvf git-lfs-linux-amd64-v3.4.0.tar.gz; then
        echo "Error: Failed to extract the tarball"
        exit 1
fi

# Enter the extracted directory
cd git-lfs-3.4.0 || { echo "Error: Failed to enter the extracted directory"; exit 1; }

# Check if the user has root privileges before installing
if [ "$(id -u)" != "0" ]; then
        echo "Error: Installation requires root privileges" 1>&2
        exit 1
fi

# Install Git LFS
./install.sh

echo "Git LFS installation successful"
```

# 监控

## atop

``` bash
yum install atop -y
```



# 第三方库

## libatomic

``` bash
yum install libatomic -y
```

## libreadline

``` bash
yum install readline-devel
```




