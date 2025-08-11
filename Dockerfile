FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get install -y build-essential clang-format clang-tidy pkg-config python3 python3-pip libboost-all-dev liblua5.3-dev doctest-dev cmake git
RUN rm -rf /var/lib/apt/lists/*

RUN git clone --depth 1 https://github.com/quickjs-ng/quickjs.git /tmp/quickjs \
 && make -C /tmp/quickjs \
 && make -C /tmp/quickjs install \
 && rm -rf /tmp/quickjs
ENV PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH

WORKDIR /app
COPY . .

RUN make -j$(nproc)

CMD ["/bin/bash"]
