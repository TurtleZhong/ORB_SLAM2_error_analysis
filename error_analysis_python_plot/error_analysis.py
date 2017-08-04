#! /usr/bin/python
# Filename: error_analysis.py
# Author: TurtleZhong 2017.08.04 in DM

import matplotlib.pyplot as plt
import numpy as np

seq_step_dir = ['error_analysis_3', 'error_analysis_4',
                'error_analysis_5', 'error_analysis_6',
                'error_analysis_7', 'error_analysis_8',
                'error_analysis_9', 'error_analysis_10']
seq_file_name = ['11.txt', '12.txt', '13.txt', '14.txt', '15.txt',
                 '16.txt', '17.txt', '18.txt', '19.txt', '20.txt',
                 '21.txt']

error_analysis_dir = '/home/m/KITTI/devkit/cpp/results/ORB/error_analysis/'

plot_color = ['red',
              'green',
              'pink',
              'orchid',
              'tan',
              'violet',
              'yellow',
              'blue']
label = ['step = 3', 'step = 4', 'step = 5', 'step = 6', 'step = 7', 'step = 8', 'step = 9', 'step = 10']


seq_error = []
seq_error_step = []


for seq in range(10, 11):
    for step in seq_step_dir:
        # print error_analysis_dir + step + '/' + seq_file_name[seq]
        error_file_dir = error_analysis_dir + step + '/' + seq_file_name[seq]
        error_file = open(error_file_dir)
        error = error_file.readlines()
        seq_error_step_index = []
        for tmp in error:
            tmp = tmp.split()
            list(tmp)
            line = [tmp[0], tmp[2], tmp[3], tmp[4], tmp[5]]
            seq_error_step_index.append(line)
        # print len(seq_error_step_index)
        seq_error_step.append(seq_error_step_index)
        # print len(seq_error_step)
    seq_error.append(seq_error_step)
    print len(seq_error)

# now we get all the errors and next
# we ned to plot it using matplotlib and numpy

x = []
y = []
z = []

for seq in range(0, 1):
    for error in seq_error_step:
        c_frame = []
        r_err = []
        t_err = []
        for item in error:
            c_frame.append(item[0])
            r_err.append(item[1])
            t_err.append(item[2])
        x.append(np.array(c_frame))
        y.append(np.array(r_err))
        z.append(np.array(t_err))
    print len(x)




# show the data
for index in range(0,8):
    plt.xlabel('frame sequence 10', size=14)
    plt.ylabel('translation error', size=14)
    plt.plot(x[index], z[index], color=plot_color[index], label=label[index])
    plt.legend(loc='upper left')
plt.savefig('sequence10.png', dpi=600)
plt.show()
print 'save image suscessfully!'











