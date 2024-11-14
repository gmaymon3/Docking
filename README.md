# Spacecraft Docking w/ Computer Vision

This program uses OpenCV, canny edge detection (with blur), and Hough Circle detection to find the docking ring on a spacecraft. The RPO distance between the spacecraft is required as an input to seek the correct size circle. It is manually generated here. 

An example output looks something like below. The yellow circle represent where the algorithm believes the docking ring to be located. 

![Docking Example](https://github.com/user-attachments/assets/1078e118-cec4-43bf-aa90-fc63fbe2d2e3)
