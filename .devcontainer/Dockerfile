FROM ubuntu:23.10
LABEL Description="Build environment"

ENV HOME /root

SHELL ["/bin/bash", "-c"]

# RUN apt-get update && apt-get -y --no-install-recommends install \
#     build-essential \
#     curl \
#     ca-certificates \
#     git
# 
# RUN /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
# RUN (echo; echo 'eval "$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)"') >> /root/.profile
# RUN eval "$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)"; brew install gcc cmake

RUN apt-get -y update
RUN apt-get -y install libterm-ui-perl
RUN apt-get -y install \
    gcc \
    g++ \
    cmake \
    git \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    gdb \
    autoconf \
    autoconf-archive

RUN git clone --depth=1 https://github.com/Microsoft/vcpkg.git
RUN ./vcpkg/bootstrap-vcpkg.sh

ENV CMAKE_TOOLCHAIN_FILE /vcpkg/scripts/buildsystems/vcpkg.cmake
ENV CXX /usr/bin/g++
ENV CC /usr/bin/gcc

VOLUME /src
WORKDIR /src

CMD ["tail", "-f", "/dev/null"]
