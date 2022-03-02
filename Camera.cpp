#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        this->cameraDirection = glm::normalize(cameraPosition - cameraTarget);

        //TODO - Update the rest of camera parameters


        this->cameraFrontDirection = glm::normalize(cameraPosition - cameraTarget);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraUpDirection, cameraFrontDirection));
        this->cameraUpDirection = glm::cross(cameraFrontDirection, cameraRightDirection);

    }
    glm::vec3 Camera::getCameraTarget()
    {
        return cameraTarget;
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        // return glm::lookAt(cameraPosition, cameraTarget, cameraDirection);

        return glm::lookAt(cameraPosition, cameraFrontDirection + cameraPosition, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        switch (direction) {
        case MOVE_FORWARD:
            cameraPosition += cameraFrontDirection * speed;
            break;

        case MOVE_BACKWARD:
            cameraPosition -= cameraFrontDirection * speed;
            break;

        case MOVE_RIGHT:
            cameraPosition += cameraRightDirection * speed;
            break;

        case MOVE_LEFT:
            cameraPosition -= cameraRightDirection * speed;
            break;
        case MOVE_UP:
            cameraPosition += cameraUpDirection * speed;
            break;
        case MOVE_DOWN:
            cameraPosition -= cameraUpDirection * speed;
            break;
        }
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)); // Note that we convert the angle to radians first
        direction.z = sin(glm::radians(yaw));
        direction.y = sin(glm::radians(pitch));
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        yaw = -90.0f;

        cameraFrontDirection = glm::normalize(direction);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
     
    }
    void Camera::vizualizareScena() {
        // set the camera
        this->cameraPosition = glm::vec3(-50.0f, 500.0f, 480.0f);

        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
    }
    
}