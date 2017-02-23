

#include <iostream>
#include <vector>
#include <memory>


#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/ext.hpp>

extern "C" {
  #include "bridge.h"
  #include <OpenGL/gl3.h>
}

#include "Engine.h"


struct Polygon {
  
  long num_vertices;
  float radius;
  GLuint vertex_buffer;

  Polygon(long v, float r) {
    num_vertices = v;
    radius = r;
  }
  
  void Transfer() {
    float r = radius;
    float v = num_vertices;
    float m = 2.0f * glm::pi<float>() / v;
    
    std::vector<float> data;
    for (long i = 0; i < v; i++) {
      float x = cosf(m * i) * r;
      float y = sinf(m * i) * r;
      data.push_back(x);
      data.push_back(y);
      data.push_back(0);
    }
    
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    
    vertex_buffer = vbo;
  }

  void Draw() {
    // draw a triangle
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(
      0, // layout number - declared at vertex shader
      3, // [X,Y,Z]
      GL_FLOAT,
      GL_FALSE,
      0,
      (void*)0
    );
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, num_vertices);
    glDisableVertexAttribArray(0);
  }
};

struct EngineState {
  GLuint program;
  GLint uniform_color;
  GLint uniform_matrix;
  
  float timer = 0;
  
  std::unique_ptr<Polygon> polygon;
};


// ------------------

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
  
  long num_vertices = 3;
  float radius = 0.5;

  std::unique_ptr<Polygon> p(new Polygon(num_vertices, radius));

  // generate a vertex buffer
  p->Transfer();
  
  state_->polygon = std::move(p);
  
  GLuint program = static_cast<GLuint>(CreateShaderProgram());
  glUseProgram(program);
  
  state_->program = program;
  state_->uniform_color  = glGetUniformLocation(program, "fragmentColor");
  state_->uniform_matrix = glGetUniformLocation(program, "MVP"); 
}


void Engine::RenderGL(float frame_time) {
  
  float timer = state_->timer;
  
  // clear our canvas
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  // transforms
  float rotation_speed = 1.0f; 
  glm::vec3 axis(0.0f, 0.0f, 1.0f);
  glm::mat4 rotation = glm::rotate(timer * glm::pi<float>() * rotation_speed, axis);
  
  float translation_speed = 0.25f;
  glm::vec3 offset(0.0f, 0.5f, 0.0f);
  offset = glm::rotateZ(offset, timer * glm::pi<float>() * translation_speed);
  
  // rotate first, then translate 
  glm::mat4 mvp = glm::translate(offset) * rotation;
  
  // apply model transform 
  glUniformMatrix4fv(state_->uniform_matrix, 1, GL_FALSE, glm::value_ptr(mvp));

  // specify color 
  glUniform4f(state_->uniform_color, 0, 1.0, 0, 1.0);

  // draw a polygon  
  state_->polygon->Draw();
  
  // calculate elapsed time 
  state_->timer += frame_time;
}




