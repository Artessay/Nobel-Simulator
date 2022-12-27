﻿# README

12.18 zly update:
1. 怎么使skybox支持透明图层
2. 怎么使爆炸闪光持续更长时间

### 图形渲染管线

由于OpenGL中的事物存在于3D空间中，但屏幕和窗口却是2D像素数组、因此，OpenGL的大部分工作都是关于把3D坐标转变为适应屏幕的2D像素。

OpenGL的**图形渲染管线（Graphics Pipeline）**就是负责管理3D坐标转为2D坐标的处理过程的。图形渲染管线可以被划分为两个主要部分：第一部分把你的3D坐标转换为2D坐标，第二部分是把2D坐标转变为实际的有颜色的像素。

（@todo LearnOpenGL上的流程图）



在上图中，我们给出了OpenGL中图形渲染管线工作的基本流程，根据该流程，我们可以对其中的七个步骤逐一说明。

首先，我们以数组的形式传递3D坐标作为图形渲染管线的输入。这个数组叫做**顶点数据(Vertex Data)**。顶点数据是一系列顶点的集合。一个顶点(Vertex)是一个3D坐标的数据的集合。而顶点数据是用顶点属性(Vertex Attribute)表示的，它可以包含任何我们想用的数据。

图形渲染管线的第一个部分是**顶点着色器(Vertex Shader)**，它把一个单独的顶点作为输入。顶点着色器主要的目的是**把3D坐标转为另一种3D坐标**，并对顶点属性进行一些基本处理。

**图元装配(Primitive Assembly)**阶段将顶点着色器输出的所有顶点作为输入，并**将所有的点装配成指定图元的形状**。

图元装配阶段的输出会传递给**几何着色器(Geometry Shader)**。几何着色器把图元形式的一系列顶点的集合作为输入，它可以通过**产生新顶点构造出新的图元来生成其他形状**。

几何着色器的输出会被传入**光栅化阶段(Rasterization Stage)**，这里它会**把图元映射为最终屏幕上相应的像素**，生成供片段着色器(Fragment Shader)使用的片段(Fragment)。在片段着色器运行之前会执行**裁切(Clipping)**。裁切会丢弃超出你的视图以外的所有像素，用来提升执行效率。

**片段着色器**的主要目的是**计算一个像素的最终颜色**，这也是所有OpenGL高级效果产生的地方。通常，片段着色器包含3D场景的数据（比如光照、阴影、光的颜色等等），这些数据可以被用来计算最终像素的颜色。

在所有对应颜色值确定以后，最终的对象将会被传到最后一个阶段，我们叫做**Alpha测试**和**混合(Blending)**阶段。这个阶段检测片段的对应的深度和模板(Stencil)值，用它们来判断这个像素是其它物体的前面还是后面，决定是否应该丢弃。这个阶段也会检查定义了一个物体透明度的alpha值并对物体进行混合(Blend)。所以，即使在片段着色器中计算出来了一个像素输出的颜色，在渲染多个图形的时候最后的像素颜色也可能完全不同。

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

12.24 0:17 rkj update:
1. 添加sphere和cylinder与炸弹的碰撞。

12.25 14:35 rkj update:
1. 添加box、machine与炸弹的碰撞。
2. 修了一点之前碰撞检测的bug。


### 环境光照

即使在黑暗的情况下，世界中也仍然存在背景光亮，所以我们所看到的环境永远不会是完全黑暗的。为了模拟环境光照的场景，我们会使用一个环境光照常量，它永远会给物体一些颜色。

在我的太阳系使用一个很小的常量颜色，添加到物体片段的最终颜色中，在即便场景中没有直接的光源里，物体也能看起来存在有一些发散的光。
$$
f_{ambient}(i, o) = constant
$$
为了实现添加环境光照，我们只需要将光的颜色乘以一个很小的常量环境因子，再乘以物体的颜色，然后将最终结果作为片段的颜色即可。

~~~glsl
// ambient
vec3 ambient = ambientStrength * lightColor;
~~~



### 漫反射光照

对于物体而言，当太阳光照在物体上时，其视觉上最显著的分量就是漫反射形成的光照了。其中，物体正对着太阳光的半球会被照亮为白色，而背对着太阳光。

在计算漫反射光照时，我们首先计算光源和片段位置之间的方向向量，即光照的方向向量。接着，我们对顶点法向量和光照向量进行点乘，计算光源对当前片段实际的漫反射影响。结果值再乘以光的颜色，得到漫反射分量。两个向量之间的角度越大，漫反射分量就会越小。

考虑到在实际中，通常漫反射的光照强度与光线的入射角相关。因此，我们可以设计一个与光线入射角的余弦值大小成正比的漫反射模型。
$$
f_{diffuse}(i, o) = <i, n> * constant
$$
如果两个向量之间的角度大于90度，点乘的结果就会变成负数，这样会导致漫反射分量变为负数。为此，我们使用max函数返回两个参数之间较大的参数，从而保证漫反射分量不会变成负数。在实现上可以向片段着色器中加入如下代码：

~~~glsl
// diffuse 
vec3 norm = normalize(Normal);
vec3 lightDir = normalize(lightPos - FragPos);
float diff = max(dot(norm, lightDir), 0.0);
vec3 diffuse = diff * lightColor;
~~~



### 镜面光照

镜面光照是为了模拟有光泽物体上面出现的亮点。镜面光照的颜色相比于物体的颜色会更倾向于光的颜色。

在我们的实现中，镜面光照决定于光的方向向量和物体的法向量以及观察方向。镜面光照决定于表面的反射特性。如果我们把物体表面设想为一面镜子，那么镜面光照最强的地方就是我们看到表面上反射光的地方；而如果用户从偏离出射点的角度去观察时，光线的亮度会急剧地衰减。

在这种情形下，假设入射光向量与视线方向向量的角平分线所对应的半程向量为$h$，我们所设计的物体镜面反射模型为：
$$
f_{specular}(i,o) = (<n, h>)^{shininess} * constant
$$
在实现上，这也是可以直接通过GLSL语言在片段着色器上直接附上相关内容即可。从程序实现的简便角度来看，我们可以调用一些GLSL语言中的函数来计算会更加方便一些。

~~~glsl
// specular
vec3 viewDir = normalize(viewPos - FragPos);
vec3 reflectDir = reflect(-lightDir, norm); 
float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
vec3 specular = specularStrength * spec * lightColor;
~~~

