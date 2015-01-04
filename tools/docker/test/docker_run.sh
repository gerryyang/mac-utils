#!/bin/sh

sample_job=$(docker run -d busybox /bin/sh -c "while true; do echo Docker; sleep 1; done")

sleep 10
docker logs $sample_job
docker stop -t 2 $sample_job

