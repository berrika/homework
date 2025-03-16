# 概述
基于半边数据结构的三维建模项目，能基于 mvfs, mev, mef, kemr 和 kfmrh 5个基本欧拉操作和 sweep 扫掠操作生成对应的立方体，用Vulkan实现可视化。

# 环境
ide：visual studio 2022
图形api：vulkan
外部库：glfw3.3.8,glm0.9.9.8,imgui,fmt

# 项目结构
- 结构说明
  - 源文件：在 ./cpp 和 ./head 目录下
    - HalfEdgeDataStructure.h：半边数据结构
    - EulerOperation.cpp / EulerOperation.h：欧拉操作
    - Sweep.cpp / Sweep.h：扫琼操作
    - first_app.cpp / first_app.h: 主循环
    - keyboard_movement_controller.cpp / keyboard_movement_controller.h: 键鼠操作
    - lve_buffer.cpp / lve_buffer.h: 缓存
    - lve_camera.cpp / lve_camera.h: 摄像机
    - lve_descriptors.cpp / lve_descriptors.h: 描述器设置
    - lve_device.cpp / lve_device.hpp: 硬件设置
    - lve_frame_info.h: 帧信息
    - lve_game_object.cpp / lve_game_object.h: 对象信息
    - lve_model.cpp / lve_model.h: 模型信息
    - lve_pipeline.cpp / lve_pipeline.h: 管线设置
    - lve_renderer.cpp / lve_renderer.h: 渲染设置
    - lve_swap_chain.cpp / lve_swap_chain.hpp: 交换链设置
    - lve_utils.h： hash
    - lve_window.cpp / lve_window.h: 窗口信息
    - ModelingData.cpp / ModelingData.h： 欧拉操作模型信息
    - point_light_system.cpp / point_light_system.h: 光源信息
    - simple_render_system.cpp / simple_render_system.h： 渲染系统
    - test.cpp： main函数
  - 其他文件：在 ./resource 目录下
    - 顶点着色器文件：simple_shader.vert, point_light.vert以及相应spv文件
    - 片段着色器文件：simple_shader.frag, point_light.frag以及相应spv文件
    - 批处理文件： compile.bat转换着色器为spv文件
    - 地形模型文件： quad.obj

# 操作
- 键盘操作：
    - WASDQE： 相机前后左右上下移动
    - ↑↓←→： 相机相应方向旋转
- 鼠标操作：
    - 按住左键后相机相应方向旋转
- imgui操作：
    - 模型的相应变换以及重设
<img width="315" alt="1699787206293" src="https://github.com/berrika/homework/assets/144405640/cf3a8c9a-9a38-4908-ad9c-58c06c566ada">

# 输入
    in.txt为输入。第1行为1个整数n，表示底面n个环；然后输入n个环的数据，每个环的数据格式为：第1行为1个整数m，表示该环有m个顶点，之后的m行按逆时针顺序每行输入1个点的坐标，x、y、z坐标以空格分隔；最后一行输入扫成操作使用的向量。

# 输出
图像界面形成相应的带孔模型和渲染  
<img width="397" alt="1699787242672" src="https://github.com/berrika/homework/assets/144405640/38268571-9bcb-49b9-bc44-068a2774fbd2">
