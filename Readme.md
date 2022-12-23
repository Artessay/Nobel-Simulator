﻿# README

12.18 zly update:
1. 怎么使skybox支持透明图层
2. 怎么使爆炸闪光持续更长时间


### 炸弹设计

真实世界中，当我们向外抛出一个物体时，物体受到重力的作用将会以抛物线的形式向外飞出，最终落到地上。在我们的《诺贝尔模拟器》中，我们对于炸弹抛出时弹道轨迹的设计也打算尽可能模拟真实世界中的物理场景，设计出炸弹抛出时近似真实世界中的物理轨迹。

在玩家操纵诺贝尔抛出炸弹时，玩家有一个观察视角表示观察方向的向量$Front$，这也是玩家的前进方向。我们假设，在玩家抛出炸弹时，抛出炸弹的运动轨迹位于玩家前进方向向量$Front$和始终指向世界上方的方向向量$Up$所张成的平面上，这样，我们可以将炸弹在三维空间中抛出轨迹的问题转换到二维平面上来求解。

玩家运动过程中，我们会同时记录下玩家前进方向在水平面上的投影$Ahead$向量。其中，$Ahead = normalize(Front.x, 0, Front.z)$，设$Front$向量和$Ahead$向量之间的夹角为$theta$

我们假定，无论玩家以怎样的角度抛出炸弹，炸弹在抛出时的初速度$v_0$都是保持不变的，炸弹抛出时的位置和方向即为玩家所在的位置和玩家的朝向。

$$
\begin{equation}
    \left\{
        \begin{array}{r}
            x = x_0 + v_0 \cos ( \theta ) t \\
            y = y_0 + v_0 \sin ( \theta ) t - \cfrac{1}{2} g t^2
        \end{array}
    \right.
\end{equation}
$$

12.23 12:16 rkj update:
1. 把炸弹2秒后爆炸取消了，改成碰到地面再炸，不然向天上扔炸弹时会在天上爆炸。
2. 将相机最低位置和炸弹最低位置都改低了一点，不然容易还没碰到物体就炸。
3. 把Cylinder、Box、Sphere都改成了单位大小。
