# Real-Time Collaborative WhiteBoard

Created a Collaborative WhiteBoard Using OpenGL and socket Programming in C.

![Screenshot](Screenshot.png)


#Usage

gcc Server.c -lglut -lGL -lGLU -lm -pthread -o server
./server

gcc Client.c -lglut -lGL -lGLU -lm -pthread -o client
./client
