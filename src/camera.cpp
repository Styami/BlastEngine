#include "camera.hpp"
#include <glm/ext.hpp>

Camera::Camera() :
    m_position(0),
    m_rotation(0),
    m_side(1, 0, 0),
    m_up(0, 1, 0),
    m_forward(0, 0, -1),
    m_aspect(16.f/9),
    m_fov(glm::radians(45.f)),
    radianPerMillisec(glm::radians(0.1)),
    distPerMillisec(0.01)
{}

Camera::Camera(float aspect, float fov, const glm::vec3& position) :
    m_position(position),
    m_rotation(glm::vec3(0)),
    m_side(1, 0, 0),
    m_up(0, 1, 0),
    m_forward(0, 0, 1),
    m_aspect(aspect),
    m_fov(fov),
    radianPerMillisec(glm::radians(0.1)),
    distPerMillisec(0.01)
{}

Camera::Camera(const Camera& another) :
    m_position(another.m_position),
    m_rotation(another.m_rotation),
    m_side(another.m_side),
    m_up(another.m_up),
    m_forward(another.m_forward),
    m_aspect(another.m_aspect),
    m_fov(another.m_fov),
    radianPerMillisec(another.radianPerMillisec),
    distPerMillisec(another.distPerMillisec)
{}

Camera& Camera::operator=(const Camera& another) {
    m_position = another.m_position;
    m_rotation = another.m_rotation;
    m_up = another.m_up;
    m_forward = another.m_forward;
    m_side = another.m_side;
    m_aspect = another.m_aspect;
    m_fov = another.m_fov;
    radianPerMillisec = another.radianPerMillisec;
    distPerMillisec = another.distPerMillisec;

    return *this;
}

void Camera::forward(double deltaTime) {
    float dist = distPerMillisec * deltaTime;
    m_position -= m_forward * dist;
}

void Camera::left(double deltaTime) {
    float dist = distPerMillisec * deltaTime;
    m_position -= m_side * dist;
}

void Camera::right(double deltaTime) {
    float dist = distPerMillisec * deltaTime;
    m_position += m_side * dist;
}

void Camera::backward(double deltaTime) {
    float dist = distPerMillisec * deltaTime;
    m_position += m_forward * dist;
}

void Camera::upward(double deltaTime) {
    float angle = radianPerMillisec * deltaTime;
    float oldRotation = m_rotation.x;
    m_rotation.x = glm::clamp((m_rotation.x + angle), glm::radians(-90.f), glm::radians(90.f));
    angle = m_rotation.x - oldRotation; //permit to know the real angle to rotate
    m_up = glm::rotate(glm::mat4(1), angle, m_side) * glm::vec4(m_up, 1);

    m_forward = glm::normalize(glm::cross(m_side, m_up));
}

void Camera::downward(double deltaTime) {
    float angle = radianPerMillisec * deltaTime;
    float oldRotation = m_rotation.x;
    m_rotation.x = glm::clamp((m_rotation.x - angle), glm::radians(-90.f), glm::radians(90.f));
    angle = m_rotation.x - oldRotation;
    m_up = glm::rotate(glm::mat4(1), angle, m_side) * glm::vec4(m_up, 1);

    m_forward = glm::normalize(glm::cross(m_side, m_up));
}

void Camera::setAspect(float aspect) {
    m_aspect = aspect;
}

glm::mat4 Camera::getView() const {
    glm::mat4 view = {
        {m_side, 0},
        {m_up, 0},
        {m_forward, 0},
        {0, 0, 0, 1}
    };

    return glm::translate(glm::transpose(view), -m_position);
}

glm::mat4 Camera::getProj() const {
    glm::mat4 perspective = glm::perspective(m_fov, m_aspect, 0.1f, 10.f);
    perspective[1][1] *= -1;
    return perspective;
}
