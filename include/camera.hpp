#ifndef CAMERA_HPP
#define CAMERA_HPP
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

class Camera {
    public:
        Camera();
        Camera(float aspect, float fov, const glm::vec3& position = glm::vec3(0, 0, 1));
        Camera(const Camera& another);
        Camera& operator=(const Camera& another);
        void forward(double deltaTime);
        void left(double deltaTime);
        void right(double deltaTime);
        void backward(double deltaTime);
        void upward(double deltaTime);
        void downward(double deltaTime);
        void setAspect(float aspect);

        glm::mat4 getView() const;
        glm::mat4 getProj() const;

    private:
        glm::vec3 m_position;
        glm::vec3 m_rotation;
        glm::vec3 m_side;
        glm::vec3 m_up;
        glm::vec3 m_forward;
        float m_aspect;
        float m_fov;
        float radianPerMillisec;
        float distPerMillisec;

};

#endif