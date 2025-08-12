FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get install -y build-essential clang-format clang-tidy pkg-config python3 python3-pip libboost-all-dev liblua5.3-dev ninja-build doctest-dev git curl cmake
RUN rm -rf /var/lib/apt/lists/*

RUN git clone --depth 1 https://github.com/quickjs-ng/quickjs.git /tmp/quickjs \
 && make -C /tmp/quickjs \
 && make -C /tmp/quickjs install \
 && rm -rf /tmp/quickjs

RUN git clone --depth 1 https://github.com/ggerganov/llama.cpp.git /tmp/llama.cpp
RUN cmake -S /tmp/llama.cpp -B /tmp/llama.cpp/build -G Ninja -DLLAMA_BUILD_EXAMPLES=OFF -DLLAMA_BUILD_TESTS=OFF -DBUILD_SHARED_LIBS=OFF -DLLAMA_CURL=OFF -DCMAKE_INSTALL_PREFIX=/usr/local \
    && cmake --build /tmp/llama.cpp/build -j && cmake --install /tmp/llama.cpp/build

ENV PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH

WORKDIR /app
COPY . .

RUN make -j$(nproc)

CMD ["/bin/bash"]
