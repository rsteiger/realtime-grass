OpenGL Grass Simulation
=======================

I received several requests from people who watched my video on YouTube 
for the source, so here it is. If you missed the video you can check it out here:

http://www.youtube.com/watch?v=A0scwWn_Xsc

Compiling And Running
---------------------

I've only tested this on OS X and Ubuntu Linux, and that was a few years ago. 
If you want to use Windows, then God help you.

This source was written a few years ago, and I did it in a hurry. So if things seem confusing and
sloppy that's because they are.

I can bet you this won't compile on the first try and you'll have to make a few changes yourself,
but hey, it's here, that's all you can ask for. If it's giving you extaordinarily long error messages,
make sure you have glut (freeglut), glu, and glew installed.

    $> make
    $> ./grass

There you go. If you're on a mac, then moving around should work nicely with the arrow keys
and panning around the window will work with the mouse. If you're on linux, trying to move around
will ruin everything.
