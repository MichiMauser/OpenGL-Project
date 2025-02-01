#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        //TODO
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //TODO

        return glm::lookAt(cameraPosition, cameraTarget, this->cameraUpDirection);
    }
    glm::vec3 Camera::getCameraPosition() {
       

        return cameraPosition;
    }
       
    glm::vec3 Camera::getCameraTarget() {


        return  glm::normalize(cameraTarget - cameraPosition);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {

        glm::vec3 cameraDir = glm::normalize(cameraTarget - cameraPosition);

        glm::vec3 x_axis = glm::normalize(glm::cross(cameraDir, cameraUpDirection));

        if (direction == MOVE_LEFT) {
            cameraPosition -= x_axis * speed;
            cameraTarget -= x_axis * speed;


        }
        else if (direction == MOVE_RIGHT) {
            cameraPosition += x_axis * speed;
            cameraTarget += x_axis * speed;
        }
        else if (direction == MOVE_FORWARD) {

            cameraPosition += cameraDir * speed;
            cameraTarget += cameraDir * speed;
        }
        else if (direction == MOVE_BACKWARD) {
            cameraPosition -= cameraDir * speed;
            cameraTarget -= cameraDir * speed;
        }

    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        glm::vec3 cameraDir;

        cameraDir = glm::normalize(direction);

        cameraTarget = cameraDir + cameraPosition;
    }
}
