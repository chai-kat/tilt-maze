# Description
This is our Mini Project: a tilting maze game which simulates the handheld ball mazes you often find as prizes at carnivals, etc.

As part of this project we have implemented automatic maze generation, and a rudimentary library/headfor the LSM6DSO accelerometer/gyroscope sensor.

In addition, we've written a basic graphics library for the Soloman Systech SSD1306 display controller, which powers the 128x32 OLED display on the ChipKIT Basic I/O Shield.

# Demonstration
A visualization of the display functions can be found in the MazeGenCollated Jupyter Notebook, and the following video shows the ball moving according to the ChipKIT board/accelerometer tilt.

https://user-images.githubusercontent.com/17334965/156857018-122f21ee-c25b-437e-9cdb-dc94ad912707.mov

# Contributions
Chaitanya Katpatal: I2C functions, LSM6DSO library, maze generation code <br />
Adam Mehdi: Screen drawing functionality, graphics/collision logic

Buck, Jamis. “Maze Generation: Eller's Algorithm.” The Buckblog, 29 Dec. 2010, https://weblog.jamisbuck.org/2010/12/29/maze-generation-eller-s-algorithm.
J. Buck, “Maze Generation: Eller's Algorithm,” The Buckblog, 29-Dec-2010. [Online]. Available: https://weblog.jamisbuck.org/2010/12/29/maze-generation-eller-s-algorithm. [Accessed: 05-Mar-2022]. 
