
# Kubernetes
---
[TOC]


## 1 Docs

https://v1-7.docs.kubernetes.io/docs/tutorials/kubernetes-basics/

## 2 Codes

https://github.com/kubernetes/kubernetes

https://github.com/kubernetes/minikube

## 3 Start Interactive Tutorial

https://v1-7.docs.kubernetes.io/docs/tutorials/kubernetes-basics/cluster-interactive/

### 3.1 Create a Cluster

### 3.2 Deploy an App

### 3.3 Explore Your App

### 3.4 Expose Your App Publicly

### 3.5 Scale Your App

### 3.6 Update Your App


## 4 Glossary

* Minikube

A Kubernetes cluster can be deployed on either physical or virtual machines. To get started with Kubernetes development, you can use Minikube. Minikube is a lightweight Kubernetes implementation that creates a VM on your local machine and deploys a simple cluster containing only one node. Minikube is available for Linux, macOS, and Windows systems. The Minikube CLI provides basic bootstrapping operations for working with your cluster, including start, stop, status, and delete. For this tutorial, however, you'll use a provided online terminal with Minikube pre-installed.

* Master 

The Master is responsible for managing the cluster. The master coordinates all activities in your cluster, such as scheduling applications, maintaining applications' desired state, scaling applications, and rolling out new updates.

* Node

A Pod always runs on a Node. A Node is a worker machine in Kubernetes and may be either a virtual or a physical machine, depending on the cluster. Each Node is managed by the Master. A Node can have multiple pods, and the Kubernetes master automatically handles scheduling the pods across the Nodes in the cluster. The Master's automatic scheduling takes into account the available resources on each Node.

Every Kubernetes Node runs at least:

* `Kubelet`, a process responsible for communication between the Kubernetes Master and the Nodes; it manages the Pods and the containers running on a machine.
* `A container runtime (like Docker, rkt)` responsible for pulling the container image from a registry, unpacking the container, and running the application.

![pic](https://d33wubrfki0l68.cloudfront.net/5cb72d407cbe2755e581b6de757e0d81760d5b86/a9df9/docs/tutorials/kubernetes-basics/public/images/module_03_nodes.svg)


* Pod

A Pod models an application-specific "logical host" and can contain different application containers which are relatively tightly coupled. For example, a Pod might include both the container with your Node.js app as well as a different container that feeds the data to be published by the Node.js webserver. The containers in a Pod share an IP Address and port space, are always co-located and co-scheduled, and run in a shared context on the same Node.

Pods are the atomic unit on the Kubernetes platform. When we create a Deployment on Kubernetes, that Deployment creates Pods with containers inside them (as opposed to creating containers directly). Each Pod is tied to the Node where it is scheduled, and remains there until termination (according to restart policy) or deletion. In case of a Node failure, identical Pods are scheduled on other available Nodes in the cluster.

![pic](https://d33wubrfki0l68.cloudfront.net/fe03f68d8ede9815184852ca2a4fd30325e5d15a/98064/docs/tutorials/kubernetes-basics/public/images/module_03_pods.svg)

* Services

A Service in Kubernetes is an abstraction which defines a logical set of Pods and a policy by which to access them. Services enable a loose coupling between dependent Pods. A Service is defined using YAML (preferred) or JSON, like all Kubernetes objects. The set of Pods targeted by a Service is usually determined by a LabelSelector (see below for why you might want a Service without including selector in the spec).

* Rolling updates

Users expect applications to be available all the time and developers are expected to deploy new versions of them several times a day. In Kubernetes this is done with rolling updates. Rolling updates allow Deployments' update to take place with zero downtime by incrementally updating Pods instances with new ones. The new Pods will be scheduled on Nodes with available resources.

## 命令

### kubectl

```
kubectl version

kubectl cluster-info
kubectl get nodes
kubectl get namespaces


# Deplying an App
kubectl run kubernetes-bootcamp --image=gcr.io/google-samples/kubernetes-bootcamp:v1 --port=8080
kubectl get deployments

# create a proxy that will forward communication into the cluster-wide, private network.
kubectl proxy
curl http://localhost:8001/version

# get the pod name
export POD_NAME=$(kubectl get pods -o go-template --template '{{range .items}}{{.metadata.name}}{{"\}}{{end}}')
curl http://localhost:8001/api/v1/proxy/namespaces/default/pods/$POD_NAME/

# list resources
kubectl get pods

# show detailed information about a resource
kubectl describe pods

# print the logs from a container in a pod
kubectl logs $POD_NAME

# execute a command on a container in a pod
kubectl exec $POD_NAME env

# start a bash session in the Pod's container
kubectl exec -it $POD_NAME bash

# list the current Services from our cluster
kubectl get services

# to create a new service and expose it to external traffic we'll use the expose command with NodePort as parameter
kubectl expose deployment/kubernetes-bootcamp --type="NodePort" --port 8080

# Node external port assigned
export NODE_PORT=$(kubectl get services/kubernetes-bootcamp -o go-template='{{(index .spec.ports 0).dePort}}')
curl $(minikube ip):$NODE_PORT

# see the name of the label
kubectl describe deployment

# use label to query our list of Pods
kubectl get pods -l run=kubernetes-bootcamp
kubectl get services -l run=kubernetes-bootcamp

# to apply a new label
kubectl label pod $POD_NAME app=v1
kubectl describe pods $POD_NAME
kubectl get pods -l app=v1

# delete a service
kubectl delete service -l run=kubernetes-bootcamp
kubectl get services
curl $(minikube ip):$NODE_PORT
kubectl exec -ti $POD_NAME curl localhost:8080

# scaling a deplyment
kubectl get deployments

# scale up
kubectl scale deployments/kubernetes-bootcamp --replicas=4
kubectl get pods -o wide
kubectl describe deployments/kubernetes-bootcamp
kubectl describe services/kubernetes-bootcamp
export NODE_PORT=$(kubectl get services/kubernetes-bootcamp -o go-template='{{(index .spec.ports 0).dePort}}')
# we hit a different Pod with every request. This demonstrates that the load-balancing is working.
curl $(minikube ip):$NODE_PORT

# scale down
kubectl scale deployments/kubernetes-bootcamp --replicas=2
kubectl get deployments

# update and rollback your app
kubectl get deployments
kubectl get pods
kubectl describe pods (look at the Image field)
# to update the image of the app to version 2. The command notified the Deployment to use a different image for your app and initiated a rolling update.
kubectl set image deployments/kubernetes-bootcamp kubernetes-bootcamp=jocatalin/kubernetes-bootcamp:v2

kubectl rollout status deployments/kubernetes-bootcamp
kubectl describe pods (look at the Image field)

# simulate error: ErrImagePull
# There is no image called v10 in the repository. 
kubectl set image deployments/kubernetes-bootcamp kubernetes-bootcamp=gcr.io/google-samples/kubernet-bootcamp:v10
kubectl get pods

# Let's roll back to our previously working version.
kubectl rollout undo deployments/kubernetes-bootcamp
kubectl get pods
```


```
# kubectl cluster-info
Kubernetes master is running at http://localhost:8080
Heapster is running at http://localhost:8080/api/v1/namespaces/kube-system/services/heapster/proxy
KubeDNS is running at http://localhost:8080/api/v1/namespaces/kube-system/services/kube-dns/proxy
kubernetes-dashboard is running at http://localhost:8080/api/v1/namespaces/kube-system/services/kubernetes-dashboard/proxy
monitoring-grafana is running at http://localhost:8080/api/v1/namespaces/kube-system/services/monitoring-grafana/proxy
monitoring-influxdb is running at http://localhost:8080/api/v1/namespaces/kube-system/services/monitoring-influxdb/proxy
```

```
# kubectl get nodes
NAME      STATUS    AGE       VERSION
node1     Ready     1d        v1.7.1+coreos.0
node2     Ready     1d        v1.7.1+coreos.0
```

```
$ kubectl get pods
NAME                                   READY     STATUS    RESTARTS   AGE
kubernetes-bootcamp-5dbf48f7d4-k8bbc   1/1       Running   0          1m

$ kubectl describe pods
Name:           kubernetes-bootcamp-5dbf48f7d4-k8bbc
Namespace:      default
Node:           host01/172.17.0.18
Start Time:     Thu, 22 Mar 2018 03:38:32 +0000
Labels:         pod-template-hash=1869049380
                run=kubernetes-bootcamp
Annotations:    <none>
Status:         Running
IP:             172.18.0.2
Controlled By:  ReplicaSet/kubernetes-bootcamp-5dbf48f7d4
Containers:
  kubernetes-bootcamp:
    Container ID:   docker://9c4350764c438fac59115d892a04a16f3c26c54634bbf5eaf62be7861e10dcd0
    Image:          gcr.io/google-samples/kubernetes-bootcamp:v1
    Image ID:       docker-pullable://jocatalin/kubernetes-bootcamp@sha256:0d6b8ee63bb57c5f5b6156f446b3bc3b3c143d233037f3a2f00e279c8fcc64af
    Port:           8080/TCP
    State:          Running
      Started:      Thu, 22 Mar 2018 03:38:33 +0000
    Ready:          True
    Restart Count:  0
    Environment:    <none>
    Mounts:
      /var/run/secrets/kubernetes.io/serviceaccount from default-token-vdkpt (ro)
Conditions:
  Type           Status
  Initialized    True
  Ready          True
  PodScheduled   True
Volumes:
  default-token-vdkpt:
    Type:        Secret (a volume populated by a Secret)
    SecretName:  default-token-vdkpt
    Optional:    false
QoS Class:       BestEffort
Node-Selectors:  <none>
Tolerations:     <none>
Events:
  Type     Reason                 Age              From               Message
  ----     ------                 ----             ----               -------
  Warning  FailedScheduling       2m (x3 over 2m)  default-scheduler  0/1 nodes are available: 1 NodeNotReady.
  Normal   Scheduled              2m               default-scheduler  Successfully assigned kubernetes-bootcamp-5dbf48f7d4-k8bbc to host01
  Normal   SuccessfulMountVolume  2m               kubelet, host01    MountVolume.SetUp succeeded forvolume "default-token-vdkpt"
  Normal   Pulled                 2m               kubelet, host01    Container image "gcr.io/google-samples/kubernetes-bootcamp:v1" already present on machine
  Normal   Created                2m               kubelet, host01    Created container
  Normal   Started                2m               kubelet, host01    Started container
```

```
$ export POD_NAME=$(kubectl get pods -o go-template --template '{{range .items}}{{.metadata.name}}{{"\}}{{end}}')
$ echo Name of the Pod: $POD_NAME
Name of the Pod: kubernetes-bootcamp-5dbf48f7d4-k8bbc

$ curl http://localhost:8001/api/v1/proxy/namespaces/default/pods/$POD_NAME/
Hello Kubernetes bootcamp! | Running on: kubernetes-bootcamp-5dbf48f7d4-k8bbc | v=1
```

```
$ kubectl get deployments
NAME                  DESIRED   CURRENT   UP-TO-DATE   AVAILABLE   AGE
kubernetes-bootcamp   1         1         1            1           25s
$ kubectl scale deployments/kubernetes-bootcamp --replicas=4
deployment "kubernetes-bootcamp" scaled
$ kubectl get deployments
NAME                  DESIRED   CURRENT   UP-TO-DATE   AVAILABLE   AGE
kubernetes-bootcamp   4         4         4            1           2m
$ kubectl get pods -o wide
NAME                                   READY     STATUS    RESTARTS   AGE       IP           NODE
kubernetes-bootcamp-5dbf48f7d4-5vxm5   1/1       Running   0          54s       172.18.0.5   host01
kubernetes-bootcamp-5dbf48f7d4-j7kdx   1/1       Running   0          54s       172.18.0.6   host01
kubernetes-bootcamp-5dbf48f7d4-n4rcm   1/1       Running   0          54s       172.18.0.7   host01
kubernetes-bootcamp-5dbf48f7d4-s6qj6   1/1       Running   0          3m        172.18.0.3   host01
```

```
$ kubectl scale deployments/kubernetes-bootcamp --replicas=2
deployment "kubernetes-bootcamp" scaled
$ kubectl get deployments
NAME                  DESIRED   CURRENT   UP-TO-DATE   AVAILABLE   AGE
kubernetes-bootcamp   2         2         2            2           13m
$ kubectl get pods -o wide
NAME                                   READY     STATUS        RESTARTS   AGE       IP           NODE
kubernetes-bootcamp-5dbf48f7d4-5vxm5   1/1       Running       0          11m       172.18.0.5   host01
kubernetes-bootcamp-5dbf48f7d4-j7kdx   1/1       Terminating   0          11m       172.18.0.6   host01
kubernetes-bootcamp-5dbf48f7d4-n4rcm   1/1       Terminating   0          11m       172.18.0.7   host01
kubernetes-bootcamp-5dbf48f7d4-s6qj6   1/1       Running       0          13m       172.18.0.3   host01
```
