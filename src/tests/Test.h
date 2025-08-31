#pragma once
struct GLFWwindow ;

class Test {
public :
   virtual void onUpdate(GLFWwindow* window) = 0 ;
   virtual void onRender() = 0 ;
   virtual void onImguiRender() = 0;
};