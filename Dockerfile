FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get install -y build-essential clang-format clang-tidy pkg-config python3 python3-pip libboost-all-dev liblua5.3-dev git
RUN rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Apply C++ formatting and build
RUN make format
RUN make

CMD ["/bin/bash"]