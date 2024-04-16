# FGMovement

Movement System being used for Factory Game. It's a work in progress but feel free to use it on your Unreal Game or for learning. It will become more battle tested as time goes on.

## Important Notices

**You must be on Unreal Engine 5.4 or above to use FGMovement.**

The following project config (Independent Interpolation) is currently required in `Config/DefaultNetworkPrediction.ini` to fix jittering:
```
[/Script/NetworkPrediction.NetworkPredictionSettingsObject]
Settings=(PreferredTickingPolicy=Independent,ReplicatedManagerClassOverride=None,FixedTickFrameRate=60,bForceEngineFixTickForcePhysics=True,SimulatedProxyNetworkLOD=ForwardPredict,FixedTickInterpolationBufferedMS=100,IndependentTickInterpolationBufferedMS=100,IndependentTickInterpolationMaxBufferedMS=250,FixedTickInputSendCount=6,IndependentTickInputSendCount=6,MaximumRemoteInputFaultLimit=6)
```
![image](https://github.com/daftsoftware/FGMovement/assets/9282017/69601534-a7be-4964-a175-ecc37f1f3ed9)
