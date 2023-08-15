#pragma once

#include "Components/Visual/VisualTrapezoidal.h"

class Shark {
public:
    float txPos = 0.0f;
    float tyPos = 0.0f;
    float tzPos = 0.0f;

    float getRandomFloat(float min, float max) {
        return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
    }
    // Constructor
    Shark(float x) {
        // Initialize member variable
        xPos = getRandomFloat(20.0f, 80.0f);
yPos = getRandomFloat(10.0f, 50.0f);
zPos = getRandomFloat(20.0f, 80.0f);
        txPos = xPos + 0.9f;
        tyPos = yPos + 0.9f;
        tzPos = zPos + 0.9f;
        taille = 0.005625f * x * 3;
        sharkPos = glm::vec3(xPos, yPos, zPos);
        targetPos = glm::vec3(txPos, tyPos, tzPos);
        travel = targetPos - sharkPos;
        travel = glm::normalize(travel);
        float a = getRandomFloat(0.0f, 1.0f);
        float b = getRandomFloat(0.0f, 1.0f);
        float c = getRandomFloat(0.0f, 1.0f);
        color = glm::vec3(a, b, c);
        color1 = glm::vec3(b, a, b);
        color2 = glm::vec3(c, b, a);
        coin = getRandomFloat(-1.0f, 1.0f);

    }

    void DrawFromMatrix(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, const glm::mat4 &_transformMatrix,  float _time, int _renderMode, const Shader::Material *_material) {
            update();

        update();

        Shader::Material head_material = Shader::Material();
        Shader::Material body_material = Shader::Material();
        Shader::Material tail_material = Shader::Material();

        head_material.shader = _material->shader;
        body_material.shader = _material->shader;
        tail_material.shader = _material->shader;

        if (coin > 0.0f) {
            head_material.color = color;
            body_material.color = color1;
            tail_material.color = color2;
        } else if (coin < 0.0f) {
            head_material.color = color2;
            body_material.color = color;
            tail_material.color = color1;
        } else {
            head_material.color = color1;
            body_material.color = color2;
            tail_material.color = color;
        }

        initialRotation = glm::rotate(_transformMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        initialScale = glm::scale(_transformMatrix, glm::vec3(taille, taille, taille));
        initialRotation1 = glm::rotate(_transformMatrix, glm::radians(glm::sin(time / (1.25f)) * 10), glm::vec3(1.0f, 0.0f, 0.0f));
        initialRotation2 = glm::rotate(_transformMatrix, glm::radians(-glm::sin(time / (1.50f)) * 9), glm::vec3(1.0f, 0.0f, 0.0f));
        initialRotation3 = glm::rotate(_transformMatrix, glm::radians(-glm::sin(time / (1.75f)) * 9), glm::vec3(1.0f, 0.0f, 0.0f));
        initialRotation4 = glm::rotate(_transformMatrix, glm::radians(-glm::sin(time / (2.0f)) * 8), glm::vec3(1.0f, 0.0f, 0.0f));

        glm::vec3 rightVector = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), travel);
        rightVector = glm::normalize(rightVector);
        glm::vec3 realUp = glm::cross(travel, rightVector);
        glm::mat4 trap = initialPosition * translate(glm::mat4(1.0f), sharkPos)
                         * targetRotation * travelRotation * initialRotation * worldRotation * initialScale * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        //trapezoidal.DrawFromMatrix(_viewProjection, _cameraPosition, trap, _time, _renderMode, &current_material);

        glm::mat4 trapeze = trap * translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(5.75f, 2.75f, 4.0f));
        trapezoidal.DrawFromMatrix(_viewProjection, _cameraPosition, trapeze, _time, _renderMode, &head_material);

        glm::mat4 trapezeChild = trap * translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

        glm::mat4 br1 = trapezeChild * initialRotation1 * translate(glm::mat4(1.0f), glm::vec3(0.0f, -9.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(10.5f, 10.75f, 10.0f));
        cube.DrawFromMatrix(_viewProjection, _cameraPosition, br1, _time, _renderMode, &body_material);

        glm::mat4 cubechild = trapezeChild * initialRotation1 * translate(glm::mat4(1.0f), glm::vec3(0.0f, -10.0f, 0.0f));

        glm::mat4 wing = cubechild * translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 7.5f)) * glm::rotate(glm::mat4(1.0f), glm::radians(75.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.75f, 3.5f, 2.0f));
        trapezoidal.DrawFromMatrix(_viewProjection, _cameraPosition, wing, _time, _renderMode, &head_material);

        wing = cubechild * translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -7.5f)) * glm::rotate(glm::mat4(1.0f), glm::radians(105.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.75f, 3.5f, 2.0f));
        trapezoidal.DrawFromMatrix(_viewProjection, _cameraPosition, wing, _time, _renderMode, &head_material);

        br1 = cubechild * initialRotation2 * translate(glm::mat4(1.0f), glm::vec3(0.0f, -9.5f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(10.5f, 10.75f, 10.0f));
        cube.DrawFromMatrix(_viewProjection, _cameraPosition, br1, _time, _renderMode, &body_material);

        glm::mat4 trapezeChild1 = cubechild * initialRotation2 * translate(glm::mat4(1.0f), glm::vec3(0.0f, -9.5f, 0.0f));

        br1 = trapezeChild1 * initialRotation3 * translate(glm::mat4(1.0f), glm::vec3(0.0f, -7.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(5.25f, 1.75f, 3.5f));
        trapezoidal.DrawFromMatrix(_viewProjection, _cameraPosition, br1, _time, _renderMode, &tail_material);

        glm::mat4 trapezeChild2 = trapezeChild1 * initialRotation3 * translate(glm::mat4(1.0f), glm::vec3(0.0f, -12.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        br1 = trapezeChild2 * initialRotation4 * translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.5f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(5.25f, 2.5f, 1.5f));
        trapezoidal.DrawFromMatrix(_viewProjection, _cameraPosition, br1, _time, _renderMode, &body_material);
    }
    // Member function
    void update() {

        dt = glfwGetTime() - time;
        time += dt;
        sharkPos += travel * speed / 100.0f;


        if (glm::abs(cAngleZ - tAngleZ) > 0.1) {
            if (cAngleZ - tAngleZ > 0) {
                cAngleZ = cAngleZ - rotationSpeed * dt;
            }
            else {
                cAngleZ = cAngleZ + rotationSpeed * dt;
            }
        }

        if (glm::abs(cAngleY - tAngleY) > 0.1) {
            if (cAngleY - tAngleY > 0) {
                cAngleY -= rotationSpeed * dt;
            }
            else {
                cAngleY += rotationSpeed * dt;
            }
        }

        // cAngleY = 0.0f;
        cAngleZ = 0.0f;
        travelRotation = glm::rotate(glm::mat4(1.0f), glm::radians(cAngleZ), glm::vec3(0.0f, 0.0f, 1.0f));
        targetRotation = glm::rotate(glm::mat4(1.0f), glm::radians(cAngleY), glm::vec3(0.0f, 1.0f, 0.0f));

        if (glm::distance(sharkPos, targetPos) <= 1.0f) {
            targetPos = glm::vec3(getRandomFloat(-30.0f, 30.0f), getRandomFloat(-30.0f, 30.0f), getRandomFloat(-30.0f, 30.0f));
            travel = targetPos - sharkPos;
            travel = glm::normalize(travel);

            float angle = glm::acos(travel.x);
            angle = glm::degrees(angle);
            if (travel.z < 0) {
                angle *= -1.0f;
            }
            tAngleY = angle;
            std::cout << "T angle Y: " << tAngleY << " T angle Z: " << tAngleZ << std::endl;
        }


    }

private:
    float xPos = 1.0f;
    float yPos = 1.0f;
    float zPos = 1.0f;
    float time = 0.0f;
    
    glm::mat4 initialRotation1 = glm::mat4(1.0);
    glm::mat4 initialRotation2 = glm::mat4(1.0);
    glm::mat4 initialRotation3 = glm::mat4(1.0);
    glm::mat4 initialRotation4 = glm::mat4(1.0);
    glm::mat4 initialRotation = glm::mat4(1.0);
    glm::mat4 targetRotation = glm::mat4(1.0);
    glm::mat4 travelRotation = glm::mat4(1.0);
    glm::mat4 initialPosition = glm::mat4(1.0);
    glm::mat4 worldRotation = glm::mat4(1.0f);
    glm::mat4 initialScale = glm::mat4(1.0f);
    
    VisualTrapezoidal trapezoidal = VisualTrapezoidal(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.40, 0.85, 1.25f));
    VisualCube cube = VisualCube();
    
    float taille = 1.0f;
    
    glm::vec3 sharkPos;
    glm::vec3 travel;
    
    float speed = 4.0f;
    float rotationSpeed = 90.0f;
    float dt = 0.0f;
    
    glm::vec3 targetPos;
    glm::vec3 targetAngles;
    glm::vec3 currentAngles;
    
    float cAngleY = 0.0f;
    float cAngleZ = 0.0f;
    float tAngleY = 0.0f;
    float tAngleZ = 0.0f;
    float coin;
    
    glm::vec3 color;
    glm::vec3 color1;
    glm::vec3 color2;
};

class Fish {
public:
    float txPos = 2.0f;
    float tyPos = 2.0f;
    float tzPos = 2.0f;
    
    float getRandomFloat(float min, float max) {
        return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
    }
    
    // Constructor
    Fish(float x) {
        // Initialize member variable
        xPos = getRandomFloat(20.0f, 80.0f);
yPos = getRandomFloat(10.0f, 50.0f);
zPos = getRandomFloat(20.0f, 80.0f);
        txPos = xPos + 0.9f;
        tyPos = yPos + 0.9f;
        tzPos = zPos + 0.9f;
        taille = 0.005625f * x;
        fishPos = glm::vec3(xPos, yPos, zPos);
        targetPos = glm::vec3(txPos, tyPos, tzPos);
        travel = targetPos - fishPos;
        travel = glm::normalize(travel);
        float a = getRandomFloat(0.0f, 1.0f);
        float b = getRandomFloat(0.0f, 1.0f);
        float c = getRandomFloat(0.0f, 1.0f);
        color = glm::vec3(a, b, c);
        color1 = glm::vec3(b, a, b);
        color2 = glm::vec3(c, b, a);
        coin = getRandomFloat(-1.0f, 1.0f);
    }
    
    glm::vec3 getPosition() {
        return fishPos;
    }

    void DrawFromMatrix(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, const glm::mat4 &_transformMatrix,  float _time, int _renderMode, const Shader::Material *_material) {
        update();

        Shader::Material head_material = Shader::Material();
        Shader::Material body_material = Shader::Material();
        Shader::Material tail_material = Shader::Material();

        head_material.shader = _material->shader;
        body_material.shader = _material->shader;
        tail_material.shader = _material->shader;

        if (coin > 0.0f) {
            head_material.color = color;
            body_material.color = color1;
            tail_material.color = color2;
        } else if (coin < 0.0f) {
            head_material.color = color2;
            body_material.color = color;
            tail_material.color = color1;
        } else {
            head_material.color = color1;
            body_material.color = color2;
            tail_material.color = color;
        }

        time = glfwGetTime();
        initialRotation = glm::rotate(_transformMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        initialScale = scale(_transformMatrix, glm::vec3(taille, taille, taille));
        initialRotation1 = glm::rotate(_transformMatrix, glm::radians(glm::sin(time * 2) * 20), glm::vec3(1.0f, 0.0f, 0.0f));
        initialRotation2 = glm::rotate(_transformMatrix, glm::radians(-glm::sin(time * 2 + 20) * 20), glm::vec3(0.0f, 1.0f, 0.0f));
        initialRotation3 = glm::rotate(_transformMatrix, glm::radians(-glm::sin(time * 2 + 40) * 20), glm::vec3(0.0f, 1.0f, 0.0f));
        initialRotation4 = glm::rotate(_transformMatrix, glm::radians(-glm::sin(time * 2 + 60) * 20), glm::vec3(1.0f, 0.0f, 0.0f));

        glm::mat4 trap = initialPosition * translate(glm::mat4(1.0f), fishPos)
                    * travelRotation * targetRotation * initialRotation * worldRotation * initialScale * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        glm::mat4 trapeze = trap * translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(3.75f, 3.75f, 1.0f));

        trapezoidal.DrawFromMatrix(_viewProjection, _cameraPosition, trapeze, _time, _renderMode, &head_material);

        glm::mat4 trapezeChild = trap * translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 br1 = trapezeChild * initialRotation1 * translate(glm::mat4(1.0f), glm::vec3(0.0f, -6.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 7.75f, 5.0f));
        cube.DrawFromMatrix(_viewProjection, _cameraPosition, br1, _time, _renderMode, &body_material);

        glm::mat4 trapezeChild1 = trapezeChild * initialRotation1 * translate(glm::mat4(1.0f), glm::vec3(0.0f, -6.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        br1 = trapezeChild1 * initialRotation2 * translate(glm::mat4(1.0f), glm::vec3(-4.5f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 7.75f, 5.0f));
        cube.DrawFromMatrix(_viewProjection, _cameraPosition, br1, _time, _renderMode, &body_material);

        glm::mat4 trapezeChild2 = trapezeChild1 * initialRotation2 * translate(glm::mat4(1.0f), glm::vec3(-4.5f, 0.0f, 0.0f));

        trapeze = trapezeChild2 * initialRotation3 * translate(glm::mat4(1.0f), glm::vec3(-7.5f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(4.75f, 4.75f, 2.0f));
        trapezoidal.DrawFromMatrix(_viewProjection, _cameraPosition, trapeze, _time, _renderMode, &head_material);

        glm::mat4 trapezeChild3 = trapezeChild2 * initialRotation3 * translate(glm::mat4(1.0f), glm::vec3(-7.5f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        trapeze = trapezeChild3 * initialRotation4 * translate(glm::mat4(1.0f), glm::vec3(0.0f, 6.75f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(3.75f, 1.0f, 1.0f));
        trapezoidal.DrawFromMatrix(_viewProjection, _cameraPosition, trapeze, _time, _renderMode, &tail_material);
    }
    
    // Member function
    void update() {
        //time = glfwGetTime();
        dt = glfwGetTime() - time;
        time += dt;
        fishPos += travel * speed / 100.0f;

        if (glm::abs(cAngleZ - tAngleZ) > 0.1) {
            if (cAngleZ - tAngleZ > 0) {
                cAngleZ = cAngleZ - rotationSpeed * dt;
            }
            else {
                cAngleZ = cAngleZ + rotationSpeed * dt;
            }
        }
        
        if (glm::abs(cAngleY - tAngleY) > 0.1) {
            if (cAngleY - tAngleY > 0) {
                cAngleY -= rotationSpeed * dt;
            }
            else {
                cAngleY += rotationSpeed * dt;
            }
        }

        cAngleZ = 0.0f;
        travelRotation = glm::rotate(glm::mat4(1.0f), glm::radians(cAngleZ), glm::vec3(0.0f, 0.0f, 1.0f));
        targetRotation = glm::rotate(glm::mat4(1.0f), glm::radians(cAngleY), glm::vec3(0.0f, 1.0f, 0.0f));

        if (glm::distance(fishPos, targetPos) <= 1.0f) {
            targetPos = glm::vec3(getRandomFloat(-30.0f, 30.0f), getRandomFloat(-30.0f, 30.0f), getRandomFloat(-30.0f, 30.0f));
            travel = targetPos - fishPos;
            travel = glm::normalize(travel);

            float angle = glm::acos(travel.x);
            angle = glm::degrees(angle);
            if (travel.z < 0) {
                angle *= -1.0f;
            }
            tAngleY = angle;
            std::cout << "T angle Y: " << tAngleY << " T angle Z: " << tAngleZ << std::endl;
        }
    }

private:
    float xPos = 1.0f;
    float yPos = 1.0f;
    float zPos = 1.0f;
    float time = 0.0f;
    
    glm::mat4 initialRotation1 = glm::mat4(1.0);
    glm::mat4 initialRotation2 = glm::mat4(1.0);
    glm::mat4 initialRotation3 = glm::mat4(1.0);
    glm::mat4 initialRotation4 = glm::mat4(1.0);
    glm::mat4 initialRotation = glm::mat4(1.0);
    glm::mat4 initialPosition = glm::mat4(1.0);
    glm::mat4 worldRotation = glm::mat4(1.0f);
    glm::mat4 targetRotation = glm::mat4(1.0);
    glm::mat4 travelRotation = glm::mat4(1.0);
    glm::mat4 initialScale = glm::mat4(1.0f);
    
    VisualTrapezoidal trapezoidal = VisualTrapezoidal(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.40, 0.85, 1.25f));
    VisualCube cube = VisualCube();
    
    float taille = 1.0f;
    
    glm::vec3 fishPos;
    glm::vec3 targetPos;
    glm::vec3 travel;
    
    float speed = 6.0f;
    float dt = 0.0f;
    float cAngleY = 0.0f;
    float cAngleZ = 0.0f;
    float tAngleY = 0.0f;
    float tAngleZ = 0.0f;
    float rotationSpeed = 90.0f;
    float coin;
    
    glm::vec3 color;
    glm::vec3 color1;
    glm::vec3 color2;
};
