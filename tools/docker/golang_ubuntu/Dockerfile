# Dockerfile

FROM ubuntu_sshd_gcc_gerry:14.04

MAINTAINER gerry

#ENV DEBIAN_FRONTEND noninteractive

RUN apt-get -y update && apt-get -y install git && apt-get -y install gccgo-go
RUN rm -rf /app && git clone https://github.com/gerryyang/goinaction.git /app
RUN cd /app/src/socket/tcpechov1 && go build tcpsvr.go

ADD run.sh /run.sh
RUN chmod 755 /*.sh

EXPOSE 9001
#WORKDIR /app
CMD ["/run.sh"]

