# User Guide

##1. Build Universal Backgroud Model
Look at the file **gen_ubm.cc**, there are some interfaces you can use. Generally, to build a ubm, you should input the data file, the dimension, the number of gaussian single models and another data file used for score normalization.(We used **zero-normalization**).

##2. Build personal Gaussian Mixture Model
We provide interfaces to save your models into files along with recover your models from them. When you begin to build personal gmm, you should first recover ubm from file, and then you can do **MAP** on it using personal data. After training you will get the personal model. A demo is like the file **gen_gmm.cc**.

##3. Do some test
You can use some features to compute scores on your model. A threshold depends on your data.


**Don't forget to watch the file ubm.h to learn more about it!**