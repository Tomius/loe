// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_COLLISION_BOUNDING_SPHERICAL_SECTOR_H_
#define ENGINE_COLLISION_BOUNDING_SPHERICAL_SECTOR_H_

#include "./bounding_box.h"
#include "../global_terrain_setting.h"

namespace engine {

class BoundingSphericalSector : public BoundingBox {
  bool invalid_ = true;

  static bool isValid(glm::vec3 model_pos) {
    return 0 <= model_pos.x && model_pos.x <= global_terrain::w &&
           0 <= model_pos.z && model_pos.z <= global_terrain::h;
  }

  static glm::vec3 transform(glm::vec3 model_pos) {
    glm::vec2 angles_degree{model_pos.x * 360 / global_terrain::w,
                            model_pos.z * 180 / global_terrain::h};
    angles_degree = glm::vec2(360-angles_degree.x, 180-angles_degree.y);
    glm::vec2 angles = 1.001f * angles_degree * float(M_PI / 180);
    float r = global_terrain::sphere_radius + model_pos.y;
    glm::vec3 cartesian = glm::vec3(
      r*sin(angles.y)*cos(angles.x),
      r*sin(angles.y)*sin(angles.x),
      r*cos(angles.y)
    );

    return cartesian;
  }

 public:
  BoundingSphericalSector() = default;

  BoundingSphericalSector(const glm::vec3& mins, const glm::vec3& maxes) {
    glm::vec3 diff = maxes - mins;
    glm::vec3 step = diff / 16.001f;
    for (float x = mins.x; x < maxes.x; x += step.x) {
      for (float y = mins.y; y < maxes.y; y += step.y) {
        for (float z = mins.z; z < maxes.z; z += step.z) {
          glm::vec3 vec{x, y, z};
          if (isValid(vec)) {
            glm::vec3 tvec{transform(vec)};
            if (invalid_) {
              mins_ = tvec;
              maxes_ = tvec;
              invalid_ = false;
            } else {
              mins_ = glm::min(tvec, mins_);
              maxes_ = glm::max(tvec, maxes_);
            }
          }
        }
      }
    }
  }

  virtual bool collidesWithSphere(const glm::vec3& center, float radius) const {
    if (invalid_) {
      return false;
    } else {
      return BoundingBox::collidesWithSphere(center, radius);
    }
  }

  virtual bool collidesWithFrustum(const Frustum& frustum) const {
    if (invalid_) {
      return false;
    } else {
      return BoundingBox::collidesWithFrustum(frustum);
    }
  }
};

}


#endif
