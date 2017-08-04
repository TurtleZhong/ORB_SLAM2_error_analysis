###数据格式说明
#####error_analysis后面的尾缀表示的是step_frame,就是当前帧跟距离stepframe的帧的误差，注意该
#####误差是与groundtruth的误差。
数据格式说明
current_frame reference_frame r_err t_err dist speed
dist 表示两帧之间的距离
speed 表示平均运行速度
两个都是基于groundtrouth计算的。
