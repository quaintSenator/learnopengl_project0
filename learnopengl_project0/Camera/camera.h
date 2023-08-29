#include <assimp/MathFunctions.h>
#ifndef CAMERA_H

#include "../Mesh/Mesh.h"
#include <cmath>
#define CAMERA_H

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    CENTER,
    LEFT_ROT,
    RIGHT_ROT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.0f;
const float SENSITIVITY = 0.05f;
const float ZOOM = 60.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    float far;
    float near;
    float aspect;
    

    glm::vec3 Target;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 4.0f, 4.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) :
    Front(glm::vec3(0.0f, 0.0f, -1.0f)),
    MovementSpeed(SPEED),
    MouseSensitivity(SENSITIVITY),
    Zoom(ZOOM),
    aspect(1920.0f/1080.0f),
    far(300.0f),
    near(0.1f)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        Target = glm::vec3(0.0);
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
    Front(glm::vec3(0.0f, 0.0f, -1.0f)),
    MovementSpeed(SPEED),
    MouseSensitivity(SENSITIVITY),
    Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        Target = glm::vec3(0.0);
        updateCameraVectors();
    }
    glm::mat4 GetCameraPerspective()
    {
        glm::mat4 perspective = glm::perspective(glm::radians(Zoom), aspect, near, far);
        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        return perspective;
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == UP)
            Position += Up * velocity;
        if (direction == DOWN)
            Position -= Up * velocity;
        if (direction == LEFT_ROT)
        {
            rotateAroundCenter(-0.8f);
        }
        if (direction == RIGHT_ROT)
        {
            rotateAroundCenter(0.8f);
        }
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }
    void SetTarget(const glm::vec3& target)
    {
        Target = target;
    }
private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
    void rotateAroundCenter(float angle)
    {
        glm::vec3 origin_cam2target = Target - Position;
        origin_cam2target.y = 0;
        origin_cam2target = glm::normalize(origin_cam2target);
        //调整位置
        glm::vec4 cameraPos = glm::vec4(Position.x, Position.y, Position.z, 1.0);
        glm::mat4 rotateMat = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0, 1.0, 0.0));
        cameraPos = rotateMat * cameraPos; 
        Position = glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z);
        //调整yaw
        /*glm::vec3 current_cam2target = Target - glm::vec3(Position.x, Position.y, Position.z);
        origin_cam2target.y = 0;
        current_cam2target = glm::normalize(current_cam2target);

        float costheta = glm::dot(origin_cam2target, current_cam2target);
        float delta_theta = acos(costheta);*/
        //acos返回的deltatheta的范围在[0,pi]
        //而实际的delta_theta应该在[-pi/2,pi/2]
        //偷个懒，我们其实知道一次rotateAround的参数只会是+-1，deltatheta 计算结果一定是一个很接近1的正值
        Yaw -= angle;
        updateCameraVectors();
    }
    
};
#endif