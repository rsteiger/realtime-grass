all:
	g++ main.cpp GLSL_helper.cpp Scene.cc Shader.cc glutMaster/glutMaster.cc glutMaster/glutWindow.cc cameraWindow.cc -DGL_GLEXT_PROTOTYPES -framework OpenGL -framework GLUT -g -framework Cocoa
