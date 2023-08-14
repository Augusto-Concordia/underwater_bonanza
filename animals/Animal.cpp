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
        xPos = getRandomFloat(-30.0f, 30.0f);
        yPos = getRandomFloat(-30.0f, 30.0f);
        zPos = getRandomFloat(-30.0f, 30.0f);
        txPos = xPos + 0.9f;
        tyPos = yPos + 0.9f;
        tzPos = zPos + 0.9f;
        taille = 0.005625f * x * 3;
        sharkPos = vec3(xPos, yPos, zPos);
        targetPos = vec3(txPos, tyPos, tzPos);
        travel = targetPos - sharkPos;
        travel = glm::normalize(travel);
        float a = getRandomFloat(0.0f, 1.0f);
        float b = getRandomFloat(0.0f, 1.0f);
        float c = getRandomFloat(0.0f, 1.0f);
        color = vec3(a, b, c);
        color1 = vec3(b, a, b);
        color2 = vec3(c, b, a);
        coin = getRandomFloat(-1.0f, 1.0f);

    }

    void Draw() {
        if (coin < 0.0f) {
            update();
            initialRotation = rotate(mat4(1.0f), glm::radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
            initialScale = scale(mat4(1.0f), vec3(taille, taille, taille));
            initialRotation1 = rotate(mat4(1.0f), glm::radians(sin(time / (1.25f)) * 10), vec3(1.0f, 0.0f, 0.0f));
            initialRotation2 = rotate(mat4(1.0f), glm::radians(-sin(time / (1.50f)) * 9), vec3(1.0f, 0.0f, 0.0f));
            initialRotation3 = rotate(mat4(1.0f), glm::radians(-sin(time / (1.75f)) * 9), vec3(1.0f, 0.0f, 0.0f));
            initialRotation4 = rotate(mat4(1.0f), glm::radians(-sin(time / (2.0f)) * 8), vec3(1.0f, 0.0f, 0.0f));

            GLuint Rendering = GL_TRIANGLES;
            int trapezoidvao1 = createTrapezoid(0.40, 0.85, 1.25f);
            glBindVertexArray(trapezoidvao1);
            vec3 rightVector = glm::cross(vec3(0.0f, 1.0f, 0.0f), travel);
            rightVector = glm::normalize(rightVector);
            vec3 realUp = glm::cross(travel, rightVector);
            mat4 trap = initialPosition * translate(mat4(1.0f), sharkPos)
                * targetRotation * travelRotation * initialRotation * initialScale * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
            GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            GLuint cc = glGetUniformLocation(shader, "customColor");
            vec3 customColor = color;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trap[0][0]);
            glDrawArrays(Rendering, 0, 36);
            mat4 trapeze = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.75f, 2.75f, 4.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);
            glDrawArrays(Rendering, 0, 36);
            mat4 trapezeChild = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
            glBindVertexArray(unitCubeVAO);

            mat4 br1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -10.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.5f, 5.75f, 5.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);
            mat4 cubechild = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -10.0f, 0.0f));

            glBindVertexArray(trapezoidvao1);


            mat4 wing = cubechild * translate(mat4(1.0f), vec3(0.0f, 0.0f, 7.5f)) * rotate(glm::mat4(1.0f), glm::radians(75.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(1.75f, 3.5f, 2.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &wing[0][0]);
            glDrawArrays(Rendering, 0, 36);

            wing = cubechild * translate(mat4(1.0f), vec3(0.0f, 0.0f, -7.5f)) * rotate(glm::mat4(1.0f), glm::radians(105.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(1.75f, 3.5f, 2.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &wing[0][0]);
            glDrawArrays(Rendering, 0, 36);

            glBindVertexArray(unitCubeVAO);

            br1 = cubechild * initialRotation2 * translate(mat4(1.0f), vec3(0.0f, -12.5f, 0.0f)) * scale(mat4(1.0f), vec3(5.5f, 5.75f, 4.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild1 = cubechild * initialRotation2 * translate(mat4(1.0f), vec3(0.0f, -12.5f, 0.0f));
            int trapezoidvao2 = createTrapezoid(0.40, 0.85, 0.1f);
            glBindVertexArray(trapezoidvao2);

            br1 = trapezeChild1 * initialRotation3 * translate(mat4(1.0f), vec3(0.0f, -11.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(5.25f, 4.75f, 3.5f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild2 = trapezeChild1 * initialRotation3 * translate(mat4(1.0f), vec3(0.0f, -12.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

            br1 = trapezeChild2 * initialRotation4 * translate(mat4(1.0f), vec3(0.0f, 1.5f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(5.25f, 2.5f, 1.5f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);

        }
        else if (coin == 0.0f) { //Very unlikely
            update();
            initialRotation = rotate(mat4(1.0f), glm::radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
            initialScale = scale(mat4(1.0f), vec3(taille, taille, taille));
            initialRotation1 = rotate(mat4(1.0f), glm::radians(sin(time / (1.25f)) * 10), vec3(1.0f, 0.0f, 0.0f));
            initialRotation2 = rotate(mat4(1.0f), glm::radians(-sin(time / (1.50f)) * 9), vec3(1.0f, 0.0f, 0.0f));
            initialRotation3 = rotate(mat4(1.0f), glm::radians(-sin(time / (1.75f)) * 9), vec3(1.0f, 0.0f, 0.0f));
            initialRotation4 = rotate(mat4(1.0f), glm::radians(-sin(time / (2.0f)) * 8), vec3(1.0f, 0.0f, 0.0f));

            GLuint Rendering = GL_TRIANGLES;
            int trapezoidvao1 = createTrapezoid(0.40, 0.85, 1.25f);
            glBindVertexArray(trapezoidvao1);
            vec3 rightVector = glm::cross(vec3(0.0f, 1.0f, 0.0f), travel);
            rightVector = glm::normalize(rightVector);
            vec3 realUp = glm::cross(travel, rightVector);
            mat4 trap = initialPosition * translate(mat4(1.0f), sharkPos)
                * travelRotation * targetRotation * initialRotation * worldRotation * initialScale * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
            GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            GLuint cc = glGetUniformLocation(shader, "customColor");
            vec3 customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trap[0][0]);
            glDrawArrays(Rendering, 0, 36);
            mat4 trapeze = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.75f, 2.75f, 4.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);
            glDrawArrays(Rendering, 0, 36);
            mat4 trapezeChild = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
            glBindVertexArray(unitCubeVAO);

            mat4 br1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -10.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.5f, 5.75f, 5.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);
            mat4 cubechild = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -10.0f, 0.0f));

            glBindVertexArray(trapezoidvao1);


            mat4 wing = cubechild * translate(mat4(1.0f), vec3(0.0f, 0.0f, 7.5f)) * rotate(glm::mat4(1.0f), glm::radians(75.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(1.75f, 3.5f, 2.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &wing[0][0]);
            glDrawArrays(Rendering, 0, 36);

            wing = cubechild * translate(mat4(1.0f), vec3(0.0f, 0.0f, -7.5f)) * rotate(glm::mat4(1.0f), glm::radians(105.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(1.75f, 3.5f, 2.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &wing[0][0]);
            glDrawArrays(Rendering, 0, 36);

            glBindVertexArray(unitCubeVAO);

            br1 = cubechild * initialRotation2 * translate(mat4(1.0f), vec3(0.0f, -12.5f, 0.0f)) * scale(mat4(1.0f), vec3(5.5f, 5.75f, 4.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild1 = cubechild * initialRotation2 * translate(mat4(1.0f), vec3(0.0f, -12.5f, 0.0f));
            int trapezoidvao2 = createTrapezoid(0.40, 0.85, 0.1f);
            glBindVertexArray(trapezoidvao2);

            br1 = trapezeChild1 * initialRotation3 * translate(mat4(1.0f), vec3(0.0f, -11.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(5.25f, 4.75f, 3.5f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild2 = trapezeChild1 * initialRotation3 * translate(mat4(1.0f), vec3(0.0f, -12.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

            br1 = trapezeChild2 * initialRotation4 * translate(mat4(1.0f), vec3(0.0f, 1.5f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(5.25f, 2.5f, 1.5f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);
        }
        else {
            update();
            initialRotation = rotate(mat4(1.0f), glm::radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
            initialScale = scale(mat4(1.0f), vec3(taille, taille, taille));
            initialRotation1 = rotate(mat4(1.0f), glm::radians(sin(time / (1.25f)) * 10), vec3(1.0f, 0.0f, 0.0f));
            initialRotation2 = rotate(mat4(1.0f), glm::radians(-sin(time / (1.50f)) * 9), vec3(1.0f, 0.0f, 0.0f));
            initialRotation3 = rotate(mat4(1.0f), glm::radians(-sin(time / (1.75f)) * 9), vec3(1.0f, 0.0f, 0.0f));
            initialRotation4 = rotate(mat4(1.0f), glm::radians(-sin(time / (2.0f)) * 8), vec3(1.0f, 0.0f, 0.0f));

            GLuint Rendering = GL_TRIANGLES;
            int trapezoidvao1 = createTrapezoid(0.40, 0.85, 1.25f);
            glBindVertexArray(trapezoidvao1);
            vec3 rightVector = glm::cross(vec3(0.0f, 1.0f, 0.0f), travel);
            rightVector = glm::normalize(rightVector);
            vec3 realUp = glm::cross(travel, rightVector);
            mat4 trap = initialPosition * translate(mat4(1.0f), sharkPos)
                * targetRotation * travelRotation * initialRotation * worldRotation * initialScale * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
            GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            GLuint cc = glGetUniformLocation(shader, "customColor");
            vec3 customColor = color;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trap[0][0]);
            glDrawArrays(Rendering, 0, 36);
            mat4 trapeze = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.75f, 2.75f, 4.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);
            glDrawArrays(Rendering, 0, 36);
            mat4 trapezeChild = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
            glBindVertexArray(unitCubeVAO);

            mat4 br1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -10.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.5f, 5.75f, 5.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color1;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);
            mat4 cubechild = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -10.0f, 0.0f));

            glBindVertexArray(trapezoidvao1);


            mat4 wing = cubechild * translate(mat4(1.0f), vec3(0.0f, 0.0f, 7.5f)) * rotate(glm::mat4(1.0f), glm::radians(75.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(1.75f, 3.5f, 2.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color1;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &wing[0][0]);
            glDrawArrays(Rendering, 0, 36);

            wing = cubechild * translate(mat4(1.0f), vec3(0.0f, 0.0f, -7.5f)) * rotate(glm::mat4(1.0f), glm::radians(105.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(1.75f, 3.5f, 2.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color1;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &wing[0][0]);
            glDrawArrays(Rendering, 0, 36);

            glBindVertexArray(unitCubeVAO);

            br1 = cubechild * initialRotation2 * translate(mat4(1.0f), vec3(0.0f, -12.5f, 0.0f)) * scale(mat4(1.0f), vec3(5.5f, 5.75f, 4.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color2;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild1 = cubechild * initialRotation2 * translate(mat4(1.0f), vec3(0.0f, -12.5f, 0.0f));
            int trapezoidvao2 = createTrapezoid(0.40, 0.85, 0.1f);
            glBindVertexArray(trapezoidvao2);

            br1 = trapezeChild1 * initialRotation3 * translate(mat4(1.0f), vec3(0.0f, -11.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(5.25f, 4.75f, 3.5f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color1;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild2 = trapezeChild1 * initialRotation3 * translate(mat4(1.0f), vec3(0.0f, -12.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

            br1 = trapezeChild2 * initialRotation4 * translate(mat4(1.0f), vec3(0.0f, 1.5f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(5.25f, 2.5f, 1.5f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);
        }
    }
    // Member function
    void update() {

        dt = glfwGetTime() - time;
        time += dt;
        sharkPos += travel * speed * dt;


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
        travelRotation = rotate(mat4(1.0f), glm::radians(cAngleZ), vec3(0.0f, 0.0f, 1.0f));
        targetRotation = rotate(mat4(1.0f), glm::radians(cAngleY), vec3(0.0f, 1.0f, 0.0f));

        if (glm::distance(sharkPos, targetPos) <= 1.0f) {
            targetPos = vec3(getRandomFloat(-5.0f, 5.0f), getRandomFloat(-5.0f, 5.0f), getRandomFloat(-5.0f, 5.0f));
            travel = targetPos - sharkPos;
            travel = glm::normalize(travel);

            float angle = glm::acos(travel.x);
            angle = degrees(angle);
            if (travel.z < 0) {
                angle *= -1.0f;
            }
            tAngleY = angle;
            std::cout << "T angle Y: " << tAngleY << " T angle Z: " << tAngleZ << std::endl;
            /*
            float angle = getRandomFloat(0.0f, 360.0f); //getRandomFloat(0.0f, 360.0f);
            travel = vec3(-cos(glm::radians(angle)), 0.0f, sin(glm::radians(angle)));
            targetPos = (getRandomFloat(10.0f, 30.0f) * travel + sharkPos);
            float fakeDistance = glm::distance(targetPos, sharkPos);
            targetPos.y = getRandomFloat(-30.0f,30.0f);
            //TERRAIN HEIGHT
            float angle2 = glm::acos(fakeDistance / glm::distance(sharkPos, targetPos));
            travel = targetPos - sharkPos;
            travel = glm::normalize(travel);

            //cAngleY = angle;
            //cAngleZ = degrees(angle2);
            tAngleY = angle;
            tAngleZ = degrees(-angle2);
            std::cout<< "T angle Y: " << tAngleY << "T angle Z: " << tAngleZ << std::endl;
            //cAngleZ = 0.0f;
            //cAngleY = tAngleY;*/
        }


    }

private:
    float xPos = 1.0f;
    float yPos = 1.0f;
    float zPos = 1.0f;
    float time = 0.0f;
    mat4 initialRotation1 = mat4(1.0);
    mat4 initialRotation2 = mat4(1.0);
    mat4 initialRotation3 = mat4(1.0);
    mat4 initialRotation4 = mat4(1.0);
    mat4 initialRotation = mat4(1.0);
    mat4 targetRotation = mat4(1.0);
    mat4 travelRotation = mat4(1.0);
    mat4 initialPosition = mat4(1.0);
    mat4 worldRotation = mat4(1.0f);
    mat4 initialScale = mat4(1.0f);
    int trapezoidvao = createTrapezoid(0.5, 0.25, 0.25f);
    int unitCubeVAO = createUnitCube();
    float taille = 1.0f;
    vec3 sharkPos;
    vec3 travel;
    float speed = 4.0f;
    float rotationSpeed = 90.0f;
    float dt = 0.0f;
    vec3 targetPos;
    vec3 targetAngles;
    vec3 currentAngles;
    float cAngleY = 0.0f;
    float cAngleZ = 0.0f;
    float tAngleY = 0.0f;
    float tAngleZ = 0.0f;
    float coin;
    vec3 color;
    vec3 color1;
    vec3 color2;
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
        xPos = getRandomFloat(-30.0f, 30.0f);
        yPos = getRandomFloat(-30.0f, 30.0f);
        zPos = getRandomFloat(-30.0f, 30.0f);
        txPos = xPos + 0.9f;
        tyPos = yPos + 0.9f;
        tzPos = zPos + 0.9f;
        taille = 0.005625f * x;
        fishPos = vec3(xPos, yPos, zPos);
        targetPos = vec3(txPos, tyPos, tzPos);
        travel = targetPos - fishPos;
        travel = glm::normalize(travel);
        float a = getRandomFloat(0.0f, 1.0f);
        float b = getRandomFloat(0.0f, 1.0f);
        float c = getRandomFloat(0.0f, 1.0f);
        color = vec3(a, b, c);
        color1 = vec3(b, a, b);
        color2 = vec3(c, b, a);
        coin = getRandomFloat(-1.0f, 1.0f);
    }
    vec3 getPosition() {
        return fishPos;
    }
    void draw() {
        if (coin < 0.0f) {
            update();
            time = glfwGetTime();
            initialRotation = rotate(mat4(1.0f), glm::radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
            initialScale = scale(mat4(1.0f), vec3(taille, taille, taille));
            initialRotation1 = rotate(mat4(1.0f), glm::radians(sin(time * 2) * 20), vec3(1.0f, 0.0f, 0.0f));
            initialRotation2 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 20) * 20), vec3(0.0f, 1.0f, 0.0f));
            initialRotation3 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 40) * 20), vec3(0.0f, 1.0f, 0.0f));
            initialRotation4 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 60) * 20), vec3(1.0f, 0.0f, 0.0f));


            GLuint Rendering = GL_TRIANGLES;
            glBindVertexArray(trapezoidvao);
            mat4 trap = initialPosition * translate(mat4(1.0f), fishPos)
                * travelRotation * targetRotation * initialRotation * worldRotation * initialScale * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
            mat4 trapeze = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 4.75f, 2.0f));
            GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            GLuint cc = glGetUniformLocation(shader, "customColor");
            vec3 customColor = color;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);

            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            glBindVertexArray(unitCubeVAO);

            mat4 br1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -8.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(2.0f, 4.75f, 2.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -8.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

            br1 = trapezeChild1 * initialRotation2 * translate(mat4(1.0f), vec3(-4.5f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(2.0f, 4.75f, 2.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild2 = trapezeChild1 * initialRotation2 * translate(mat4(1.0f), vec3(-4.5f, 0.0f, 0.0f));

            glBindVertexArray(trapezoidvao);
            trapeze = trapezeChild2 * initialRotation3 * translate(mat4(1.0f), vec3(-7.5f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 4.75f, 2.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);
            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild3 = trapezeChild2 * initialRotation3 * translate(mat4(1.0f), vec3(-7.5f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));


            trapeze = trapezeChild3 * initialRotation4 * translate(mat4(1.0f), vec3(0.0f, 2.75f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 2.0f, 1.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);

            glDrawArrays(Rendering, 0, 36);
        }
        else if (coin == 0.0f) {
            update();
            time = glfwGetTime();
            initialRotation = rotate(mat4(1.0f), glm::radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
            initialScale = scale(mat4(1.0f), vec3(taille, taille, taille));
            initialRotation1 = rotate(mat4(1.0f), glm::radians(sin(time * 2) * 20), vec3(1.0f, 0.0f, 0.0f));
            initialRotation2 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 20) * 20), vec3(0.0f, 1.0f, 0.0f));
            initialRotation3 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 40) * 20), vec3(0.0f, 1.0f, 0.0f));
            initialRotation4 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 60) * 20), vec3(1.0f, 0.0f, 0.0f));


            GLuint Rendering = GL_TRIANGLES;
            glBindVertexArray(trapezoidvao);
            mat4 trap = initialPosition * translate(mat4(1.0f), fishPos)
                * travelRotation * targetRotation * initialRotation * worldRotation * initialScale * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
            mat4 trapeze = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 4.75f, 2.0f));
            GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            GLuint cc = glGetUniformLocation(shader, "customColor");
            vec3 customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);

            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            glBindVertexArray(unitCubeVAO);

            mat4 br1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -8.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(2.0f, 4.75f, 2.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -8.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

            br1 = trapezeChild1 * initialRotation2 * translate(mat4(1.0f), vec3(-4.5f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(2.0f, 4.75f, 2.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild2 = trapezeChild1 * initialRotation2 * translate(mat4(1.0f), vec3(-4.5f, 0.0f, 0.0f));

            glBindVertexArray(trapezoidvao);
            trapeze = trapezeChild2 * initialRotation3 * translate(mat4(1.0f), vec3(-7.5f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 4.75f, 2.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);
            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild3 = trapezeChild2 * initialRotation3 * translate(mat4(1.0f), vec3(-7.5f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));


            trapeze = trapezeChild3 * initialRotation4 * translate(mat4(1.0f), vec3(0.0f, 2.75f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 2.0f, 1.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);

            glDrawArrays(Rendering, 0, 36);
        }
        else {
            update();
            time = glfwGetTime();
            initialRotation = rotate(mat4(1.0f), glm::radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
            initialScale = scale(mat4(1.0f), vec3(taille, taille, taille));
            initialRotation1 = rotate(mat4(1.0f), glm::radians(sin(time * 2) * 20), vec3(1.0f, 0.0f, 0.0f));
            initialRotation2 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 20) * 20), vec3(0.0f, 1.0f, 0.0f));
            initialRotation3 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 40) * 20), vec3(0.0f, 1.0f, 0.0f));
            initialRotation4 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 60) * 20), vec3(1.0f, 0.0f, 0.0f));


            GLuint Rendering = GL_TRIANGLES;
            glBindVertexArray(trapezoidvao);
            mat4 trap = initialPosition * translate(mat4(1.0f), fishPos)
                * travelRotation * targetRotation * initialRotation * worldRotation * initialScale * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
            mat4 trapeze = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 4.75f, 2.0f));
            GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            GLuint cc = glGetUniformLocation(shader, "customColor");
            vec3 customColor = color;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);

            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            glBindVertexArray(unitCubeVAO);

            mat4 br1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -8.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(2.0f, 4.75f, 2.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color1;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -8.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

            br1 = trapezeChild1 * initialRotation2 * translate(mat4(1.0f), vec3(-4.5f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(2.0f, 4.75f, 2.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color2;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild2 = trapezeChild1 * initialRotation2 * translate(mat4(1.0f), vec3(-4.5f, 0.0f, 0.0f));

            glBindVertexArray(trapezoidvao);
            trapeze = trapezeChild2 * initialRotation3 * translate(mat4(1.0f), vec3(-7.5f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 4.75f, 2.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color1;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);
            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild3 = trapezeChild2 * initialRotation3 * translate(mat4(1.0f), vec3(-7.5f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));


            trapeze = trapezeChild3 * initialRotation4 * translate(mat4(1.0f), vec3(0.0f, 2.75f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 2.0f, 1.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = color;
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);

            glDrawArrays(Rendering, 0, 36);
        }
    }
    void drive(GLFWwindow* window) {

        time = glfwGetTime();
        initialScale = scale(mat4(1.0f), vec3(taille, taille, taille));
        initialRotation1 = rotate(mat4(1.0f), glm::radians(sin(time * 2) * 20), vec3(1.0f, 0.0f, 0.0f));
        initialRotation2 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 20) * 20), vec3(0.0f, 1.0f, 0.0f));
        initialRotation3 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 40) * 20), vec3(0.0f, 1.0f, 0.0f));
        initialRotation4 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 60) * 20), vec3(1.0f, 0.0f, 0.0f));


        GLuint Rendering = GL_TRIANGLES;
        glBindVertexArray(trapezoidvao);
        mat4 trap = initialPosition * translate(mat4(1.0f), fishPos)
            * initialRotation * worldRotation * initialScale * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
        mat4 trapeze = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 4.75f, 2.0f));
        GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
        GLuint cc = glGetUniformLocation(shader, "customColor");
        vec3 customColor = vec3(1.0f, 0.647f, 0.0f);
        glUniform3f(cc, customColor.x, customColor.y, customColor.z);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);

        glDrawArrays(Rendering, 0, 36);

        mat4 trapezeChild = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glBindVertexArray(unitCubeVAO);

        mat4 br1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -8.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(2.0f, 4.75f, 2.0f));
        worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
        cc = glGetUniformLocation(shader, "customColor");
        customColor = vec3(1.0f, 1.0f, 1.0f);
        glUniform3f(cc, customColor.x, customColor.y, customColor.z);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
        glDrawArrays(Rendering, 0, 36);

        mat4 trapezeChild1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -8.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        br1 = trapezeChild1 * initialRotation2 * translate(mat4(1.0f), vec3(-4.5f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(2.0f, 4.75f, 2.0f));
        worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
        cc = glGetUniformLocation(shader, "customColor");
        customColor = vec3(1.0f, 0.647f, 0.0f);
        glUniform3f(cc, customColor.x, customColor.y, customColor.z);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
        glDrawArrays(Rendering, 0, 36);

        mat4 trapezeChild2 = trapezeChild1 * initialRotation2 * translate(mat4(1.0f), vec3(-4.5f, 0.0f, 0.0f));

        glBindVertexArray(trapezoidvao);
        trapeze = trapezeChild2 * initialRotation3 * translate(mat4(1.0f), vec3(-7.5f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 4.75f, 2.0f));
        worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
        cc = glGetUniformLocation(shader, "customColor");
        customColor = vec3(1.0f, 1.0f, 1.0f);
        glUniform3f(cc, customColor.x, customColor.y, customColor.z);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);
        glDrawArrays(Rendering, 0, 36);

        mat4 trapezeChild3 = trapezeChild2 * initialRotation3 * translate(mat4(1.0f), vec3(-7.5f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));


        trapeze = trapezeChild3 * initialRotation4 * translate(mat4(1.0f), vec3(0.0f, 2.75f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 2.0f, 1.0f));
        worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
        cc = glGetUniformLocation(shader, "customColor");
        customColor = vec3(1.0f, 0.647f, 0.0f);
        glUniform3f(cc, customColor.x, customColor.y, customColor.z);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);

        glDrawArrays(Rendering, 0, 36);
        //Character Movement
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // Move Racket along negative x-axis
        {
            initialPosition *= translate(mat4(1.0f), vec3(0.25f, 0.0f, 0.0f) * 0.2f);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // Move Racket along negative x-axis
        {
            initialPosition *= translate(mat4(1.0f), vec3(-0.25f, 0.0f, 0.0f) * 0.2f);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // Move Racket along positive z-axis
        {
            initialPosition *= translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.25f) * 0.2f);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // Move Racket along negative z-axis
        {
            initialPosition *= translate(mat4(1.0f), vec3(0.0f, 0.0f, -0.25f) * 0.2f);
        }

        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) // Move Racket along negative x-axis
        {
            initialPosition *= translate(mat4(1.0f), vec3(0.0f, 0.25f, 0.0f) * 0.2f);
        }
        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) // Move Racket along negative x-axis
        {
            initialPosition *= translate(mat4(1.0f), vec3(0.0f, -0.25f, 0.0f) * 0.2f);
        }

        //BONUS POINT? Racket Rotation
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // Rotate Racket positively about y-axis
        {
            initialRotation *= rotate(mat4(1.0f), 0.0872665f, vec3(0.0f, 0.25f, 0.0f) * 0.2f);
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) // Rotate Racket negatively about y-axis
        {
            initialRotation *= rotate(mat4(1.0f), -0.0872665f, vec3(0.0f, 0.25f, 0.0f) * 0.2f);
        }
    }
    // Member function
    void update() {
        //time = glfwGetTime();
        dt = glfwGetTime() - time;
        time += dt;
        fishPos += travel * speed * dt;

        if (glm::abs(cAngleZ - tAngleZ) > 0.1) {
            if (cAngleZ - tAngleZ < 0) {
                cAngleZ = cAngleZ + rotationSpeed * dt;

            }
            else {
                cAngleZ = cAngleZ - rotationSpeed * dt;
            }

        }



        if (glm::abs(cAngleY - tAngleY) > 0.1) {
            if (cAngleY - tAngleY < 0) {
                cAngleY += rotationSpeed * dt;
            }
            else {
                cAngleY -= rotationSpeed * dt;
            }


        }

        cAngleZ = 0.0f;
        travelRotation = rotate(mat4(1.0f), glm::radians(cAngleZ), vec3(0.0f, 0.0f, 1.0f));
        targetRotation = rotate(mat4(1.0f), glm::radians(cAngleY), vec3(0.0f, 1.0f, 0.0f));

        if (glm::distance(fishPos, targetPos) <= 1.0f) {
            /*float angle = getRandomFloat(-180.0f, 180.0f);
            travel = vec3(-cos(glm::radians(angle)), 0.0f, sin(glm::radians(angle)));
            targetPos = (getRandomFloat(10.0f, 30.0f) * travel + fishPos);
            float fakeDistance = glm::distance(targetPos, fishPos);
            targetPos.y = getRandomFloat(-20.0f, 20.0f);
            //TERRAIN HEIGHT
            float angle2 = glm::acos(fakeDistance / glm::distance(fishPos, targetPos));
            travel = targetPos - fishPos;
            travel = glm::normalize(travel);

            tAngleY = angle;
            tAngleZ = degrees(angle2);
            */
            targetPos = vec3(getRandomFloat(-5.0f, 5.0f), getRandomFloat(-5.0f, 5.0f), getRandomFloat(-5.0f, 5.0f));
            travel = targetPos - fishPos;
            travel = glm::normalize(travel);

            float angle = glm::acos(travel.x);
            angle = degrees(angle);
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
    mat4 initialRotation1 = mat4(1.0);
    mat4 initialRotation2 = mat4(1.0);
    mat4 initialRotation3 = mat4(1.0);
    mat4 initialRotation4 = mat4(1.0);
    mat4 initialRotation = mat4(1.0);
    mat4 initialPosition = mat4(1.0);
    mat4 worldRotation = mat4(1.0f);
    mat4 targetRotation = mat4(1.0);
    mat4 travelRotation = mat4(1.0);
    mat4 initialScale = mat4(1.0f);
    int trapezoidvao = createTrapezoid(0.5, 0.25, 0.25f);
    int unitCubeVAO = createUnitCube();
    float taille = 1.0f;
    vec3 fishPos;
    vec3 targetPos;
    vec3 travel;
    float speed = 6.0f;
    float dt = 0.0f;
    float cAngleY = 0.0f;
    float cAngleZ = 0.0f;
    float tAngleY = 0.0f;
    float tAngleZ = 0.0f;
    float rotationSpeed = 90.0f;
    float coin;
    vec3 color;
    vec3 color1;
    vec3 color2;
};
