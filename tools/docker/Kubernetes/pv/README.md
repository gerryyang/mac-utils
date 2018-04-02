
# Persistent Volume
---
[TOC]

## How to use

configure a Pod to use a PersistentVolumeClaim for storage

1. A cluster administrator creates a PersistentVolume that is backed by physical storage. The administrator does not associate the volume with any Pod.

2. A cluster user creates a PersistentVolumeClaim, which gets automatically bound to a suitable PersistentVolume.

3. The user creates a Pod that uses the PersistentVolumeClaim as storage.


PV可以理解成K8s集群中的某个网络存储中对应的一块存储，它与Volume很类似。但有以下区别：

* PV只能是网络存储，不属于任何Node，但可以在每个Node上访问。
* PV并不是定义在Pod上，而是独立于Pod之外定义。
* PV目前支持的类型包括，CephFS，NFS等。[具体](https://kubernetes.io/docs/concepts/storage/storage-classes/)

## Demo

1. Create the PersistentVolume

```
kubectl create -f https://k8s.io/docs/tasks/configure-pod-container/task-pv-volume.yaml
```

2. View information about the PersistentVolume

```
kubectl get pv task-pv-volume
```

3. Create a PersistentVolumeClaim

```
kubectl create -f https://k8s.io/docs/tasks/configure-pod-container/task-pv-claim.yaml
```

4. Look again at the PersistentVolume

```
kubectl get pv task-pv-volume
```

5. Look at the PersistentVolumeClaim

```
kubectl get pvc task-pv-claim
```

6. Create a Pod

```
kubectl create -f https://k8s.io/docs/tasks/configure-pod-container/task-pv-pod.yaml
```

7. Verify that the Container in the Pod is running

```
kubectl get pod task-pv-pod
```

8. Get a shell to the Container running in your Pod

```
kubectl exec -it task-pv-pod -- /bin/bash
```


## Refer

https://kubernetes.io/docs/tasks/configure-pod-container/configure-persistent-volume-storage/

https://github.com/kubernetes/examples/tree/master/staging/volumes/cephfs/