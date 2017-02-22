

#include <iostream>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/ext.hpp>

extern "C" {
  #include "bridge.h"
  #include <OpenGL/gl3.h>
}

#include "Engine.h"


struct EngineState {
  GLuint vertex_buffer;
  GLuint program;
  GLint uniform_color;
  GLint uniform_matrix;

  glm::mat4 mvp;
};


Engine::Engine() : state_(new EngineState()) {
  
}

Engine::~Engine() {
  delete state_;
  state_ = nullptr; 
}

void Engine::Init() {

}

void Engine::PrepareGL() {
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	static const GLfloat g_vertex_buffer_data[] = {
  	 -1.0f, -1.0f, 0.0f,
  	 1.0f, -1.0f, 0.0f,
  	 0.0f,  1.0f, 0.0f,
	};
	
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	
  state_->vertex_buffer = vertexbuffer;

  GLuint program = static_cast<GLuint>(CreateShaderProgram());
  glUseProgram(program);
  
  state_->program = program;
  state_->uniform_color  = glGetUniformLocation(program, "fragmentColor");
  state_->uniform_matrix = glGetUniformLocation(program, "MVP");
 
  glUniformMatrix4fv(state_->uniform_matrix, 1, GL_FALSE, glm::value_ptr(state_->mvp));
}


void Engine::RenderGL(float frame_time) {
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glm::vec3 axis(0.0f, 0.0f, 1.0f); 
  state_->mvp = glm::rotate(state_->mvp, frame_time * 3.141592f * 1.0f, axis);

  glUniformMatrix4fv(state_->uniform_matrix, 1, GL_FALSE, glm::value_ptr(state_->mvp));

	glUniform4f(state_->uniform_color, 0, 1.0, 0, 1.0);
	
	// draw a triangle
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, state_->vertex_buffer);
	glVertexAttribPointer(
  	0, // layout number - declared at vertex shader
   	3, 
   	GL_FLOAT,
   	GL_FALSE,
   	0, 
   	(void*)0
	);
	
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(0);

}




