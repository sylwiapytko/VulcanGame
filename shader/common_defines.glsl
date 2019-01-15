//common definitions

//light defintion
//type: 0-directional, 1-point, 2-spot
//param: light parameters

struct light_t {
  int    type;
  int    pad1, pad2, pad3;
  vec4   param;
  vec4   col_ambient;
  vec4   col_diffuse;
  vec4   col_specular;
  mat4x4 position;
};


struct UBOPerFrame_t {
    mat4 camModel;
    mat4 camView;
    mat4 camProj;
    light_t light1; 
};


