#include "VisualSphere.h"

#include <utility>
#include "Utility/Transform.hpp"

VisualSphere::VisualSphere(float radius, int subdivisions, glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale, Shader::Material _material) : VisualObject(_position, _rotation, _scale, std::move(_material))
{
    VisualSphere::radius = radius;
    VisualSphere::subdivisions = subdivisions;

    // why do we use golden ratio in icosahedron ? 
    // https://en.wikipedia.org/wiki/Regular_icosahedron
    // https://math.stackexchange.com/questions/2538184/proof-of-golden-rectangle-inside-an-icosahedron

    float golden_ratio = (1 + std::sqrt(5)) / 2; // golden ratio

    glm::vec3 vertices_arr[12] = {
        glm::vec3(-1, golden_ratio, 0), glm::vec3(1, golden_ratio, 0), glm::vec3(-1, -golden_ratio, 0), glm::vec3(1, -golden_ratio, 0),
        glm::vec3(0, -1, golden_ratio), glm::vec3(0, 1, golden_ratio), glm::vec3(0, -1, -golden_ratio), glm::vec3(0, 1, -golden_ratio),
        glm::vec3(golden_ratio, 0, -1), glm::vec3(golden_ratio, 0, 1), glm::vec3(-golden_ratio, 0, -1), glm::vec3(-golden_ratio, 0, 1)
    };

    glm::vec3 indices_arr[20] = {
        glm::vec3(0, 11, 5), glm::vec3(0, 5, 1), glm::vec3(0, 1, 7), glm::vec3(0, 7, 10), glm::vec3(0, 10, 11),
        glm::vec3(1, 5, 9), glm::vec3(5, 11, 4), glm::vec3(11, 10, 2), glm::vec3(10, 7, 6), glm::vec3(7, 1, 8),
        glm::vec3(3, 9, 4), glm::vec3(3, 4, 2), glm::vec3(3, 2, 6), glm::vec3(3, 6, 8), glm::vec3(3, 8, 9),
        glm::vec3(4, 9, 5), glm::vec3(2, 4, 11), glm::vec3(6, 2, 10), glm::vec3(8, 6, 7), glm::vec3(9, 8, 1)
    };

    // but then we have to normalize these coords to the radius
    // normalization is from https://www.ecosia.org/images?q=normalze%20vector#id=E27456AE6909F4A3FE4C6314411FAEC739E92293 normalize vectors to be out radius length

    // http://www.glprogramming.com/red/chapter02.html#name8
    // https://www.songho.ca/opengl/gl_sphere.html#icosphere
    for (int i = 0; i < 12; i++) {
        glm::vec3 v = VisualSphere::normalizeVertice(vertices_arr[i].x, vertices_arr[i].y, vertices_arr[i].z);
        vertices.push_back(v.x);
        vertices.push_back(v.y);
        vertices.push_back(v.z);
    }

    for (int i = 0; i < 20; i++) {
        indices.push_back(indices_arr[i].x);
        indices.push_back(indices_arr[i].y);
        indices.push_back(indices_arr[i].z);
    }

    // subdivide triangles
    subdivideTriangles();

    // calculate normals for vertices
    std::vector<float> normals; 
    for (int i = 0; i < vertices.size(); i+=3) {
        glm::vec3 v = glm::vec3(vertices[i], vertices[i+1], vertices[i+2]);
        glm::vec3 n = VisualSphere::computeFaceNormals(v);
        normals.push_back(n.x);
        normals.push_back(n.y);
        normals.push_back(n.z);
    }
    // calculate uv for textures for vertices
    // https://en.wikipedia.org/wiki/UV_mapping
    std::vector<float> texture; 
    for (int i = 0; i < vertices.size(); i+=3) {
        glm::vec3 v = glm::vec3(vertices[i], vertices[i+1], vertices[i+2]);
        glm::vec2 t = VisualSphere::computeVertexTexture(v);
        texture.push_back(t.x);
        texture.push_back(t.y);
    }

    std::vector<float> temp_v; 
    for (int i = 0; i < vertices.size(); i+=3) {
        glm::vec3 v = glm::vec3(vertices[i], vertices[i+1], vertices[i+2]);
        glm::vec3 n = glm::vec3(normals[i], normals[i+1], normals[i+2]);
        glm::vec2 t = glm::vec2(texture[i], texture[i+1]);
        temp_v.push_back(v.x);
        temp_v.push_back(v.y);
        temp_v.push_back(v.z);

        temp_v.push_back(n.x);
        temp_v.push_back(n.y);
        temp_v.push_back(n.z);

        temp_v.push_back(t.x);
        temp_v.push_back(t.y);
    }

    vertices = temp_v;
    VisualObject::SetupGlBuffersVerticesNormalsUvsWithIndices();
}

glm::vec3 VisualSphere::normalizeVertice(float vx, float vy, float vz) {
    float d = std::sqrt((vx * vx) + (vy * vy) + (vz * vz));
    vx = vx * (radius / (d));
    vy = vy  * (radius / (d));
    vz = vz  * (radius/ (d));
    return glm::vec3(vx, vy, vz);
}

void VisualSphere::subdivideTriangles() {
    // how many subdivisions to do ?
    for (int i = 0; i < subdivisions; i++) {
        int verticeCount = vertices.size() / 3; // Each vertex has 3 coordinates (x, y, z)
        int indiceCount = indices.size();
        int originalVerticeCount = verticeCount;
        int originalIndiceCount = indiceCount;
        // split triangle into 4 triangles
        // compute 3 new vertices by spliting half on each edge
            //          v1       
            //         / \       
            // newV12 *---* newV31
            //       / \ / \     
            //     v2---*---v3   
            //        newV23 
        for (int j = 0; j < originalIndiceCount; j+=3) {
            // indice index
            int i1 = indices[j];
            int i2 = indices[j+1];
            int i3 = indices[j+2];

            // vertices
            glm::vec3 v1 = glm::vec3(vertices[i1 * 3], vertices[i1 * 3 + 1], vertices[i1 * 3 + 2]);
            glm::vec3 v2 = glm::vec3(vertices[i2 * 3], vertices[i2 * 3 + 1], vertices[i2 * 3 + 2]);
            glm::vec3 v3 = glm::vec3(vertices[i3 * 3], vertices[i3 * 3 + 1], vertices[i3 * 3 + 2]);

            // mid points
            glm::vec3 v12 = glm::vec3((v1.x + v2.x) / 2.0f, (v1.y + v2.y) / 2.0f, (v1.z + v2.z) / 2.0f);
            glm::vec3 v23 = glm::vec3((v2.x + v3.x) / 2.0f, (v2.y + v3.y) / 2.0f, (v2.z + v3.z) / 2.0f);
            glm::vec3 v31 = glm::vec3((v3.x + v1.x) / 2.0f, (v3.y + v1.y) / 2.0f, (v3.z + v1.z) / 2.0f);

            // push normalized new point vertices to vertices vector 
            glm::vec3 new_ver[] = { v12, v23, v31 };
            for (int k = 0; k < 3; k++) {
                glm::vec3 temp = normalizeVertice(new_ver[k].x, new_ver[k].y, new_ver[k].z);
                vertices.push_back(temp.x);
                vertices.push_back(temp.y);
                vertices.push_back(temp.z);
            }

            // indices for the newly added vertices
            int i12 = verticeCount;
            int i23 = verticeCount + 1;
            int i31 = verticeCount + 2;

            // update the indices to form four new triangles
            // replace old indices with
            //          v1       
            //         / \       
            // newV12 *---* newV31
            indices[j] = i1;
            indices[j + 1] = i12;
            indices[j + 2] = i31;
            
            // newV12 *
            //       / \   
            //     v2---* 
            //        newV23 
            indices.push_back(i12);
            indices.push_back(i2);
            indices.push_back(i23);

            // newV12 *---* newV31
            //         \ /     
            //          *   
            //        newV23 
            indices.push_back(i12);
            indices.push_back(i23);
            indices.push_back(i31);

            //            * newV31
            //           / \     
            //          *---v3   
            //        newV23 
            indices.push_back(i31);
            indices.push_back(i23);
            indices.push_back(i3);
                               

            verticeCount += 3;
            indiceCount += 12;
       }
    }
}

glm::vec3 VisualSphere::computeFaceNormals(glm::vec3 v) {
    glm::vec3 n = glm::normalize(v - glm::vec3(0.0));
    return n;
}

glm::vec2 VisualSphere::computeVertexTexture(glm::vec3 v) {
    glm::vec3 n = glm::normalize(glm::vec3(0.0) - v);
    glm::vec2 t = glm::vec2(0.0);
    t.x = 0.5 + (atan2(n.z, n.x) / 2 * 3.1451f); // u
    t.y = 0.5 + (asin(n.y) / 3.1451f); //v
    return t;
}

void VisualSphere::Draw(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, int _renderMode, const Shader::Material *material)
{
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    model_matrix = Transforms::RotateDegrees(model_matrix, rotation);
    model_matrix = glm::scale(model_matrix, scale);

    DrawFromMatrix(_viewProjection, _cameraPosition, model_matrix, _renderMode, material);
}

void VisualSphere::DrawFromMatrix(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, const glm::mat4 &_transformMatrix, int _renderMode, const Shader::Material *_material)
{
    // bind the vertex array to draw
    glBindVertexArray(vertex_array_o);

    const Shader::Material *current_material = &material;

    // set the material to use on this frame
    if (_material != nullptr)
        current_material = _material;

    current_material->shader->Use();
    current_material->shader->SetModelMatrix(_transformMatrix);
    current_material->shader->SetViewProjectionMatrix(_viewProjection);

    // camera properties
    current_material->shader->SetVec3("u_cam_pos", _cameraPosition);

    // lights
    current_material->shader->ApplyLightsToShader(current_material->lights);

    // material properties
    current_material->shader->SetVec3("u_color", current_material->color);
    current_material->shader->SetFloat("u_alpha", current_material->alpha);
    current_material->shader->SetInt("u_shininess", current_material->shininess);

    // texture mapping & consumption
    current_material->texture->Use(GL_TEXTURE1);
    current_material->shader->SetFloatFast("u_texture_influence", current_material->texture_influence);
    current_material->shader->SetTexture("u_texture", 1);
    current_material->shader->SetVec2("u_texture_tiling", current_material->texture_tiling);

    // line & point properties
    glLineWidth(current_material->line_thickness);
    glPointSize(current_material->point_size);

    glDrawElements(_renderMode, indices.size(), GL_UNSIGNED_INT, nullptr);

    // clear the current texture
    current_material->texture->Clear();
}
