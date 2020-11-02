# 在图片中藏匿日记

## 编程环境
vs2019 + opencv4.5

## 说明
本人C类菜鸟，混合的C/C++编程，写得比较快，就很乱很乱

## 原理

一般的照片而言。一个像素点里面有三种颜色，分别是RGB，每种颜色使用一个字节（8bits）来存储，数值范围是0~255，

我们可以把每个颜色字节里面的最后一个比特位擦除，再写上我自己的比特信息，那么颜色数值可能会变化1，我们人眼根本看不出来区别。

这样，一个像素三个颜色分量可以为我们提供三个比特，那么三个像素可以给我们提供9个比特，去掉最后一个比特，将8比特看成一个字符操作。

……

具体的实现看源代码