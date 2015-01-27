# Dockerfile

FROM tutum/lamp:latest

MAINTAINER gerry

#ENV DEBIAN_FRONTEND noninteractive

RUN apt-get -y update && apt-get -y install gccgo-go
RUN rm -rf /app && git clone https://github.com/gerryyang/goinaction.git /app
RUN cd /app/src/http && go build http_svr_v1.go

ADD run.sh /run.sh
RUN chmod 755 /*.sh

EXPOSE 9090
#WORKDIR /app
CMD ["/run.sh"]

