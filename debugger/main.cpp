#include <memory>

#include "engine/scene.h"
#include "engine/model.h"
#include "engine/shader.h"
#include "engine/render.h"

#include "config.hpp"

#include "ray_helper.h"
#include "debugger_config.h"


/* 当前文件夹 */
std::string CUR_DIR(const std::string &file_name) {
    return fmt::format("{}/debugger/{}", ROOT_DIR, file_name);
}


class SceneD : public Scene {
    void _init() override {
        /* 模型预处理：缩小一点 */
        cornell_box_floor->set_model(scale_model);
        cornell_box_left->set_model(scale_model);
        cornell_box_light->set_model(scale_model);
        cornell_box_right->set_model(scale_model);
        cornell_box_shortbox->set_model(scale_model);
        cornell_box_tallbox->set_model(scale_model);

        /* 数据绑定 shader-diffuse */
        with(Shader, *shader_diffuse) {
            shader_diffuse->uniform_vec3_set("light_dir", {0.f, -1.f, 2.f});
            shader_diffuse->uniform_vec3_set("light_color", {0.6f, 0.6f, 0.6f});
            shader_diffuse->uniform_vec3_set("ambient", {0.4f, 0.4f, 0.4f});
        }
        shader_diffuse->set_update_per_frame([](Shader &shader) {
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
        });
        shader_diffuse->set_drawT([](Shader &shader, const Model &model, const Mesh &mesh, const glm::vec3 &color) {
            shader.uniform_mat4_set("model", model.model());
            shader.uniform_vec3_set("color", color);
        });

        /* 数据绑定：shader-rays */
        shader_rays->set_update_per_frame([](Shader &shader) {
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
        });
        shader_rays->set_drawT([](Shader &shader, const Mesh &mesh, const glm::vec3 &color) {
            shader.uniform_mat4_set("model", mesh.model());
            shader.uniform_vec3_set("ray_color", color);
        });
    }

    void _gui() override {
        ImGui::Begin("rays");

        /* 设置屏幕的坐标点 */
        ImGui::InputInt("row", &screen_row);
        ImGui::InputInt("col", &screen_col);
        if (ImGui::Button("apply row-col"))
            paths = RayPath::select_path(DB::db, screen_row, screen_col);
        ImGui::Text("path count: %lu", paths.size());

        /* 选择像素对应的第几段光路 */
        ImGui::InputInt("path idx", &path_idx);
        if (ImGui::Button("apply path idx")) {
            if (path_idx < paths.size()) {
                path = paths[path_idx];
                mesh_path = gen_ray_path_mesh(path);
            }
        }
        ImGui::Text("path node cnt: %d", path ? path->path_node_cnt : 0);

        /* 显示对应光路的信息 */
        ImGui::InputInt("path node idx", &node_idx);
        if (ImGui::Button("apply path node idx")) {
            if (path && node_idx < path->path_node_cnt) {
                node = RayNode::select_ray_node(DB::db, path->path_node_ids[node_idx]);
            }
        }
        if (node) {
            node->gui_ray_node();
        }

        ImGui::End();
    }

    void _update() override {
        shader_diffuse->update_per_frame();
        shader_rays->update_per_frame();

        /* 绘制 cornell-box */
        shader_diffuse->draw_t(*cornell_box_floor, {0.725f, 0.71f, 0.68f});
        shader_diffuse->draw_t(*cornell_box_left, {0.63f, 0.065f, 0.05f});
        shader_diffuse->draw_t(*cornell_box_light, {47.8348f, 38.5664f, 31.0808f});
        shader_diffuse->draw_t(*cornell_box_right, {0.14f, 0.45f, 0.091f});
        shader_diffuse->draw_t(*cornell_box_shortbox, {0.725f, 0.71f, 0.68f});
        shader_diffuse->draw_t(*cornell_box_tallbox, {0.725f, 0.71f, 0.68f});

        /* 绘制光线 */
        if (mesh_path)
            shader_rays->draw_t(*mesh_path, {1.f, 0.4f, 0.4f});

        /* 绘制坐标轴 */
        shader_rays->draw_t(*mesh_coord_x, {1.f, 0.f, 0.f});
        shader_rays->draw_t(*mesh_coord_y, {0.f, 1.f, 0.f});
        shader_rays->draw_t(*mesh_coord_z, {0.f, 0.f, 1.f});
    }

    static inline const float RAY_LENGTH_NO_INTER = 500;        /* 并未发生相交的光线，也要表示出来，其默认长度是多少 */

    /* 构造光路对应的模型 */
    static std::shared_ptr<Mesh> gen_ray_path_mesh(const std::shared_ptr<RayPath> &path) {
        std::vector<Line> lines;        /* 需要绘制的线段 */

        /* 遍历光路上的每一段路径 */
        for (int64_t node_id : path->path_node_ids) {
            // todo 需要判断 nullptr
            auto node = RayNode::select_ray_node(DB::db, node_id);

            /* 出射光线部分 */
            if (!node->inter_happened) {
                lines.emplace_back(node->pos_out, node->pos_out - node->wo * RAY_LENGTH_NO_INTER);
                continue;
            }
            lines.emplace_back(node->pos_out, node->pos_inter);

            /* 入射光线：来自光源 */
            if (!node->inter_light_happened) {
                lines.emplace_back(node->pos_inter, node->pos_inter + node->wi_light * RAY_LENGTH_NO_INTER);
            } else {
                lines.emplace_back(node->pos_inter, node->pos_in_light);
            }

            /* 入射光线：物体部分 */
            if (!node->inter_obj_happened) {
                lines.emplace_back(node->pos_inter, node->pos_inter + node->wi_obj * RAY_LENGTH_NO_INTER);
            } else {
                lines.emplace_back(node->pos_inter, node->pos_in_obj);
            }
        }

        /* 将模型缩小一点 */
        auto res = std::make_shared<Mesh>(lines);
        res->set_model(scale_model);
        return res;
    }


private:

    /* 光线追踪相关的属性 */
    int screen_col = 0, screen_row = 0;                 /* 像素点在屏幕的坐标 */
    std::vector<std::shared_ptr<RayPath>> paths;        /* 当前像素点对应的光线 */
    int path_idx = 0;                                   /* 当前光路在像素点的所有光路中序号是多少 */
    std::shared_ptr<RayPath> path;
    int node_idx = 0;                                   /* 当前路径在光路中的序号是多少 */
    std::shared_ptr<RayNode> node;
    std::shared_ptr<Mesh> mesh_path;                    /* 场景中的光线 */

    /* 模型缩小的变换矩阵 */
    static inline glm::mat4 scale_model = glm::scale(glm::one<glm::mat4>(), {0.01f, 0.01f, 0.01f});

    /* diffuse 着色的 shader */
    std::shared_ptr<ShaderT<glm::vec3>> shader_diffuse{new ShaderT<glm::vec3>(
            CUR_DIR("diffuse.vert"),
            CUR_DIR("diffuse.frag"))};

    /* 用于渲染线段的 shader */
    std::shared_ptr<ShaderT<glm::vec3>> shader_rays{new ShaderT<glm::vec3>(
            CUR_DIR("rays.vert"),
            CUR_DIR("rays.frag"))
    };

    /* cornell-box 的各个零件 */
    std::shared_ptr<Model> cornell_box_floor = Model::load_model(MODEL("cornell-box/floor.obj"));
    std::shared_ptr<Model> cornell_box_left = Model::load_model(MODEL("cornell-box/left.obj"));
    std::shared_ptr<Model> cornell_box_light = Model::load_model(MODEL("cornell-box/light.obj"));
    std::shared_ptr<Model> cornell_box_right = Model::load_model(MODEL("cornell-box/right.obj"));
    std::shared_ptr<Model> cornell_box_shortbox = Model::load_model(MODEL("cornell-box/shortbox.obj"));
    std::shared_ptr<Model> cornell_box_tallbox = Model::load_model(MODEL("cornell-box/tallbox.obj"));

    /* 坐标系的线段 */
    std::shared_ptr<Mesh> mesh_coord_x = std::make_shared<Mesh>(std::vector<Line>{
            {{0.f, 0.f, 0.f}, {10.f, 0.f, 0.f}}
    });
    std::shared_ptr<Mesh> mesh_coord_y = std::make_shared<Mesh>(std::vector<Line>{
            {{0.f, 0.f, 0.f}, {0.f, 10.f, 0.f}}
    });
    std::shared_ptr<Mesh> mesh_coord_z = std::make_shared<Mesh>(std::vector<Line>{
            {{0.f, 0.f, 0.f}, {0.f, 0.f, 10.f}}
    });
};


int main() {
    Render::init();
    DB::init_db(DB_PATH);
    Render::render<SceneD>();
    DB::close_db();
    Render::terminate();
    return 0;
}
