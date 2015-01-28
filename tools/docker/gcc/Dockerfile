# Dockerfile

FROM gcc:latest

MAINTAINER gerry

COPY src/* /usr/src/myapp/
WORKDIR /usr/src/myapp
RUN gcc -o myapp demo.c

CMD ["./myapp"]
