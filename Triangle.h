

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
using namespace std;
using namespace glm;


class Triangle {
  public:
    glm::vec3 origin;
    glm::vec3 other;
    glm::vec3 third;
    glm::vec3 tangent, bitangent, edge1, edge2, nm;
    glm::vec2 uv1, uv2, uv3;
    Triangle(){};
    Triangle(glm::vec3 _origin, glm::vec3 _other, glm::vec3 _third){
      origin = _origin; other = _other; third = _third;
      uv1 = glm::vec2(0.0f, 1.0f); uv2 = glm::vec2(0.0f, 0.0f); uv3 = glm::vec2(1.0f, 0.0f);

      edge1 = other - origin;
      edge2 = third - origin;
      glm::vec2 deltaUV1 = uv2 - uv1;
      glm::vec2 deltaUV2 = uv3 - uv1;

      float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
      tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
      tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
      tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

      bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
      bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
      bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

      nm = glm::normalize(glm::cross(edge1, edge2));
      // nm = glm::vec3(0.f, 0.f, 1.f);
    }

    // insertion stuff
      void insertNormal(vector<float> &vec){
        vec.push_back(nm.x);
        vec.push_back(nm.y);
        vec.push_back(nm.z);
      }
      void insertTangent(vector<float> &vec){
        vec.push_back(tangent.x);
        vec.push_back(tangent.y);
        vec.push_back(tangent.z);
      }
      void insertBitangent(vector<float> &vec){
        vec.push_back(bitangent.x);
        vec.push_back(bitangent.y);
        vec.push_back(bitangent.z);
      }
      void insertOrigin(vector<float> &vec){
        vec.push_back(origin.x);
        vec.push_back(origin.y);
        vec.push_back(origin.z);
        insertNormal(vec);
        vec.push_back(uv1.x);
        vec.push_back(uv1.y);
        insertTangent(vec);
        insertBitangent(vec);
      }
      void insertOther(vector<float> &vec){
        vec.push_back(other.x);
        vec.push_back(other.y);
        vec.push_back(other.z);
        insertNormal(vec);
        vec.push_back(uv2.x);
        vec.push_back(uv2.y);
        insertTangent(vec);
        insertBitangent(vec);
      }
      void insertThird(vector<float> &vec){
        vec.push_back(third.x);
        vec.push_back(third.y);
        vec.push_back(third.z);
        insertNormal(vec);
        vec.push_back(uv3.x);
        vec.push_back(uv3.y);
        insertTangent(vec);
        insertBitangent(vec);
      }
      void insertObject(vector<float> &vec){
        insertOrigin(vec);
        insertOther(vec);
        insertThird(vec);
      }
};