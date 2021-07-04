#ifndef RENDER_RAY_HELPER_H
#define RENDER_RAY_HELPER_H

#include <sqlite3.h>
#include <imgui.h>

// 目前只支持显示一条光线路径


/* 简单的操作数据库的代码 */
class DB {
public:
    /* 初始化数据库连接 */
    static inline void init_db(const std::string &db_path) {
        auto res = sqlite3_open(db_path.c_str(), &db);
        if (SQLITE_OK != res)
            throw std::runtime_error(fmt::format("fail to open sqlite3, path: {}", db_path));
    }

    /* 断开数据库的连接 */
    static inline void close_db() {
        sqlite3_close(db);
        db = nullptr;
    }

    static inline sqlite3 *db = nullptr;
};


/* 光路上的一个节点 */
class RayNode {
public:

    static inline const std::string TABLE_NAME = "node";        /* 数据库表的名称 */

    static std::shared_ptr<RayNode> select_ray_node(sqlite3 *db, int64_t id) {
        char **res;     /* 查询结果的一维数组：["NAME", "AGE", "aaa", "123", "bb", "34", ...] */
        int n_row, n_colume;     /* 查到的行数和列数 */
        char *err_msg;
        if (SQLITE_OK != sqlite3_get_table(db, fmt::format("SELECT * FROM {} WHERE id={}", TABLE_NAME, id).c_str(),
                                           &res, &n_row, &n_colume, &err_msg)) {
            throw std::runtime_error(fmt::format("fail to select, err msg: {}", err_msg));
        }
        return (n_row == 0) ? nullptr : format_node(res, n_colume);
    }

    /**
     * 通过查询结果来构造RayNode
     * @param select_res 查询结果，是一维数组，如：["NAME", "AGE", "aaa", "123", "bb", "34", ...]
     * @param n_colume 查询结果有几列
     */
    static std::shared_ptr<RayNode> format_node(char **select_res, int n_colume) {
        size_t idx = 0;         /* 指向查询结果的第几列 */

        /* 取得第一行第idx列的查询结果，将该值视为特定类型，并让 idx++ */
        auto f = [select_res, n_colume, &idx]() { return atof(select_res[n_colume + idx++]); };
        auto i = [select_res, n_colume, &idx]() { return atoi(select_res[n_colume + idx++]); };
        auto ll = [select_res, n_colume, &idx]() { return strtoll(select_res[n_colume + idx++], nullptr, 10); };
        auto v = [select_res, n_colume, &idx]() {
            auto res = glm::vec3(atof(select_res[n_colume + idx]),
                                 atof(select_res[n_colume + idx + 1]),
                                 atof(select_res[n_colume + idx + 2]));
            idx += 3;
            return res;
        };

        /* 通过查询结果来构造对象 */
        auto node = std::make_shared<RayNode>();

        node->id = ll();

        node->Lo = v();
        node->wo = v();
        node->pos_out = v();
        node->inter_happened = i();
        node->pos_inter = v();

        node->Li_light = v();
        node->wi_light = v();
        node->inter_light_happened = i();
        node->pos_in_light = v();

        node->Li_obj = v();
        node->wi_obj = v();
        node->inter_obj_happened = i();
        node->pos_in_obj = v();
        node->RR = f();
        node->is_obj_emission = i();

        return node;
    }


    /* 在 GUI 中显示 path node 的信息 */
    void gui_ray_node() {
        /* 出射光线的部分 */
        auto str = fmt::format("is inter: {}\n"
                               "Lo:        ({: .3f}, {: .3f}, {: .3f})\n"
                               "wo:        ({: .3f}, {: .3f}, {: .3f})\n"
                               "pos_out:   ({: .3f}, {: .3f}, {: .3f})\n"
                               "pos_inter: ({: .3f}, {: .3f}, {: .3f})\n",
                               inter_happened,
                               Lo.x, Lo.y, Lo.z,
                               wo.x, wo.y, wo.z,
                               pos_out.x, pos_out.y, pos_out.z,
                               pos_inter.x, pos_inter.y, pos_inter.z);
        ImGui::Text("%s\n", str.c_str());

        /* 光源光线的部分 */
        str = fmt::format("inter_with_light: {}\n"
                          "Li_light:     ({: .3f}, {: .3f}, {: .3f})\n"
                          "wi_light:     ({: .3f}, {: .3f}, {: .3f})\n"
                          "pos_in_light: ({: .3f}, {: .3f}, {: .3f})\n",
                          inter_light_happened,
                          Li_light.x, Li_light.y, Li_light.z,
                          wi_light.x, wi_light.y, wi_light.z,
                          pos_in_light.x, pos_in_light.y, pos_in_light.z);
        ImGui::Text("%s\n", str.c_str());

        /* 入射光线：来自物体的部分 */
        str = fmt::format("inter_with_obj: {}\n"
                          "Li_obj:     ({: .3f}, {: .3f}, {: .3f})\n"
                          "wi_obj:     ({: .3f}, {: .3f}, {: .3f})\n"
                          "pos_in_obj: ({: .3f}, {: .3f}, {: .3f})\n"
                          "RR: {}\n"
                          "is_obj_emission: {}\n",
                          inter_obj_happened,
                          Li_obj.x, Li_obj.y, Li_obj.z,
                          wi_obj.x, wi_obj.y, wi_obj.z,
                          pos_in_obj.x, pos_in_obj.y, pos_in_obj.z,
                          RR,
                          is_obj_emission);
        ImGui::Text("%s", str.c_str());
    }

    // member field
    // =========================================================================

    int64_t id;

    glm::vec3 Lo;                   /* 出射光线的 radiance */
    glm::vec3 wo;                   /* 出射光线的方向 */
    glm::vec3 pos_out;              /* 出射光线的终点 */
    bool inter_happened;            /* 是否有发生相交 */
    glm::vec3 pos_inter;            /* 相交的交点 */

    glm::vec3 Li_light;             /* 来自光源的 radiance */
    glm::vec3 wi_light;             /* 来自光源光线的方向 */
    bool inter_light_happened;      /* 是否和光源发生了相交 */
    glm::vec3 pos_in_light;         /* 与光源的交点 */

    glm::vec3 Li_obj;               /* 来自物体的 radiance */
    glm::vec3 wi_obj;               /* 来自物体的光线的方向 */
    bool inter_obj_happened;        /* 是否和物体发生了相交 */
    glm::vec3 pos_in_obj;           /* 和物体的交点 */
    float RR;                       /* 俄罗斯轮盘赌概率 */
    bool is_obj_emission;           /* 相交的物体是否是自发光的 */
};


/* 一条光路 */
class RayPath {
public:

    static inline const std::string TABLE_NAME = "path";        /* 数据库表的名称 */

    /* 查询一个像素对应的多条光路 */
    static std::vector<std::shared_ptr<RayPath>> select_path(sqlite3 *db, int row, int col) {
        /* 准备查询语句 */
        sqlite3_stmt *stmt;
        if (SQLITE_OK != sqlite3_prepare_v2(db, fmt::format("SELECT * FROM {} WHERE row={} and col={}",
                                                            TABLE_NAME, row, col).c_str(), -1, &stmt, nullptr)) {
            throw std::runtime_error("fail to select from table path.");
        }

        /* 进行查询 */
        std::vector<std::shared_ptr<RayPath>> res{};
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            auto path = std::make_shared<RayPath>();

            path->row = row;
            path->col = col;
            path->path_node_cnt = sqlite3_column_int(stmt, 2);
            path->path_node_ids = format_ids((char *) sqlite3_column_text(stmt, 3));

            res.push_back(path);
        }

        return res;
    }

    /* 获得光路上的第 idx 个 node */
    std::shared_ptr<RayNode> get_node(sqlite3 *db, size_t idx) {
        if (idx >= path_node_cnt) {
            throw std::runtime_error("bad node idx of ray-path");
        }
        return RayNode::select_ray_node(db, path_node_ids[idx]);
    }

    /**
     * 从字符串中提取多个id
     * @param str 形如："10012 12203 11102"
     */
    static std::vector<int64_t> format_ids(std::string str) {
        std::vector<int64_t> res{};

        /**
         * find(delimiter) 会返回第一次出现指定字符的地方
         * substr(start, cnt) 会提取子串 [start, start + cnt]
         * erase(start, cnt) 会删掉子串 [start, start + cnt]
         */
        size_t pos = 0;
        while ((pos = str.find(' ')) != std::string::npos) {
            auto token = str.substr(0, pos);
            str.erase(0, pos + 1);
            if (token.empty())
                continue;
            res.push_back(strtoll(token.c_str(), nullptr, 10));
        }

        return res;
    }

    // member field
    // =========================================================================

    int row, col;                           /* 光路对应屏幕像素坐标 */
    int path_node_cnt;                      /* 光路上有多少段 node */
    std::vector<int64_t> path_node_ids;     /* 光路各段 node 的 id */
};

#endif //RENDER_RAY_HELPER_H
