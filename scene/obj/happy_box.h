#ifndef RENDER_HAPPY_BOX_H
#define RENDER_HAPPY_BOX_H

#include <string>
#include <memory>

#include "../../engine/camera.h"
#include "../../engine/model.h"
#include "../../engine/texture.h"
#include "../../engine/shader.h"
#include "../../engine/light.h"
#include "../../engine/mesh.h"

#include "./cube.h"

class HappyBox : public Model {
public:
    HappyBox() {
        Cube c;
        Mesh mesh;

        for (unsigned int i = 0; i < c.indices.size(); ++i) {
            Vertex vertex;
            unsigned i3 = i * 3;
            unsigned i2 = i * 2;
            vertex.positon = glm::vec3(c.pos[i3], c.pos[i3 + 1], c.pos[i3 + 2]);
            vertex.normal = glm::vec3(c.normal[i3], c.normal[i3 + 1], c.normal[i3 + 2]);
            vertex.texcoord = glm::vec2(c.uv[i2], c.uv[i2 + 1]);
            mesh.vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < c.indices.size(); i += 3) {
            mesh.faces.emplace_back(c.indices[i], c.indices[i + 1], c.indices[i + 2]);
        }

        // texture
        auto diffuse_tex = TextureManager::texture_load(TEXTURE("container2.jpg"));
        auto specular_tex = TextureManager::texture_load(TEXTURE("container2_specular.jpg"));
        mesh.textures[diffuse] = {diffuse_tex};
        mesh.textures[specular] = {specular_tex};

        mesh.geometry_init();

        this->meshes.push_back(mesh);
    }
};

#endif //RENDER_HAPPY_BOX_H
