//#include <iostream>
//#include <opencv2/opencv.hpp>
//
//using namespace cv;
//using namespace std;
//
//void ditherImage(const Mat& inputImage, Mat& outputImage)
//{
//    std::cout << "Starting ditherImage function..." << std::endl;
//    // 将输入图像转换为灰度图
//    cvtColor(inputImage, outputImage, COLOR_BGR2GRAY);
//
//    int rows = outputImage.rows;
//    int cols = outputImage.cols;
//
//    for (int i = 0; i < rows; ++i) {
//        for (int j = 0; j < cols; ++j) {
//            uchar oldPixel = outputImage.at<uchar>(i, j);
//            uchar newPixel = (oldPixel > 127) ? 255 : 0;
//
//            outputImage.at<uchar>(i, j) = newPixel;
//
//            // 简单的抖动算法
//            int quantError = oldPixel - newPixel;
//            if (j + 1 < cols) {
//                outputImage.at<uchar>(i, j + 1) += quantError * 7 / 16;
//                std::cout << "Processing pixel at (" << i << ", " << j << ")" << std::endl;
//            }
//            if (i + 1 < rows && j - 1 >= 0) {
//                outputImage.at<uchar>(i + 1, j - 1) += quantError * 3 / 16;
//            }
//            if (i + 1 < rows) {
//                outputImage.at<uchar>(i + 1, j) += quantError * 5 / 16;
//            }
//            if (i + 1 < rows && j + 1 < cols) {
//                outputImage.at<uchar>(i + 1, j + 1) += quantError * 1 / 16;
//            }
//        }
//        std::cout << "Finished ditherImage function." << std::endl;
//    }
//}
//
//int main(int argc, char** argv) {
//    if (argc != 2) {
//        std::cout << "Usage: " << argv[0] << R"(D:\My_Learn\C++_study\Huawei\lena.png)" << std::endl;
//        std::cout << "Image path: " << argv[1] << std::endl;
//        return -1;
//    }
//
//    Mat image = imread(argv[1], IMREAD_COLOR);
//    if (image.empty()) {
//        cout << "Could not open or find the image" << endl;
//        return -1;
//    }
//
//    Mat outputImage;
//    ditherImage(image, outputImage);
//
//    imshow("Original Image", image);
//    imshow("Dithered Image", outputImage);
//
//    waitKey(0);
//
//    return 0;
//
//}

#include <iostream>
#include <vector>
#include <stack>
#include <unordered_set>
#include <chrono>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


// 美观的颜色配置
const Scalar BACKGROUND_COLOR(245, 245, 245);  // 浅灰色背景
const Scalar WALL_COLOR(80, 80, 80);          // 深灰色墙壁
const Scalar PATH_COLOR_START(40, 180, 40);    // 绿色起点
const Scalar PATH_COLOR_END(40, 40, 180);      // 蓝色终点
const Scalar PATH_COLOR_MID(180, 40, 180);     // 紫色路径
const Scalar GRID_COLOR(210, 210, 210);       // 浅灰色网格

struct Position {
    int x, y;
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

namespace std {
    template<> struct hash<Position> {
        size_t operator()(const Position& p) const {
            return hash<int>()(p.x) ^ (hash<int>()(p.y) << 1);
        }
    };
}

// 检查是否为有效移动
bool is_valid_move(int rows, int cols, int i, int j,
    const vector<vector<int>>& maze,
    const unordered_set<Position>& visited) {
    return (i >= 0 && i < rows&& j >= 0 && j < cols&&
        maze[i][j] == 1 && visited.find({ i, j }) == visited.end());
}

// 主求解函数
//vector<Position> solve_maze(const vector<vector<int>>& maze) {
//    int rows = maze.size();
//    if (rows == 0) return {};
//    int cols = maze[0].size();
//
//    // 收集所有可走的1的位置
//    vector<Position> ones;
//    for (int i = 0; i < rows; i++) {
//        if (maze[i].size() != cols) {
//            cerr << "Error: Maze is not rectangular at row " << i << endl;
//            return {};
//        }
//        for (int j = 0; j < cols; j++) {
//            if (maze[i][j] == 1) {
//                ones.emplace_back(Position{ i, j });
//            }
//        }
//    }
//    if (ones.empty()) return {};
//
//    // 定义移动方向（上、下、左、右）
//    const vector<Position> directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
//
//    // 尝试从每个1的位置作为起点
//    for (const auto& start : ones) {
//        stack<tuple<Position, vector<Position>, unordered_set<Position>>> search_stack;
//        unordered_set<Position> visited;
//        visited.insert(start);
//        search_stack.push({ start, {start}, visited });
//
//        while (!search_stack.empty()) {
//            auto current_state = search_stack.top();
//            search_stack.pop();
//            Position current_pos = get<0>(current_state);
//            vector<Position> path = get<1>(current_state);
//            unordered_set<Position> visited = get<2>(current_state);
//            // 找到路径，返回结果
//            if (path.size() == ones.size()) {
//                return path;
//            }
//
//            // 检查死胡同（剪枝优化）
//            int dead_end_count = 0;
//            for (const auto& dir : directions) {
//                int ni = current_pos.x + dir.x;
//                int nj = current_pos.y + dir.y;
//                if (!is_valid_move(rows, cols, ni, nj, maze, visited)) {
//                    dead_end_count++;
//                }
//            }
//            if (dead_end_count >= 3) continue;  // 3个方向不可走则剪枝
//
//            // 尝试四个方向
//            for (const auto& dir : directions) {
//                int ni = current_pos.x + dir.x;
//                int nj = current_pos.y + dir.y;
//                if (is_valid_move(rows, cols, ni, nj, maze, visited)) {
//                    unordered_set<Position> new_visited = visited;
//                    new_visited.insert({ ni, nj });
//                    vector<Position> new_path = path;
//                    new_path.emplace_back(Position{ ni, nj });
//                    search_stack.push({ {ni, nj}, new_path, new_visited });
//                }
//            }
//        }
//    }
//    return {};  // 无解
//}



vector<Position> solve_maze(const vector<vector<int>>& maze) {
    int rows = maze.size();
    if (rows == 0) return {};
    int cols = maze[0].size();

    vector<Position> ones;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (maze[i][j] == 1) {
                ones.emplace_back(Position{ i, j });
            }
        }
    }

    const vector<Position> directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

    for (const auto& start : ones) {
        stack<tuple<Position, vector<Position>, unordered_set<Position>>> search_stack;
        unordered_set<Position> visited;
        visited.insert(start);
        search_stack.push({ start, {start}, visited });

        while (!search_stack.empty()) {
            auto current_state = search_stack.top();
            search_stack.pop();

            Position current_pos = get<0>(current_state);
            vector<Position> path = get<1>(current_state);
            unordered_set<Position> visited = get<2>(current_state);

            if (path.size() == ones.size()) {
                return path;
            }

            // 修改后的剪枝逻辑：仅当完全被困住时才剪枝
            bool all_blocked = true;
            for (const auto& dir : directions) {
                int ni = current_pos.x + dir.x;
                int nj = current_pos.y + dir.y;
                if (is_valid_move(rows, cols, ni, nj, maze, visited)) {
                    all_blocked = false;
                    break;
                }
            }
            if (all_blocked) continue;

            for (const auto& dir : directions) {
                int ni = current_pos.x + dir.x;
                int nj = current_pos.y + dir.y;
                if (is_valid_move(rows, cols, ni, nj, maze, visited)) {
                    unordered_set<Position> new_visited = visited;
                    new_visited.insert({ ni, nj });
                    vector<Position> new_path = path;
                    new_path.emplace_back(Position{ ni, nj });
                    search_stack.push({ {ni, nj}, new_path, new_visited });
                }
            }
        }
    }
    return {};
}

void visualize_maze(const vector<vector<int>>& maze, const vector<Position>& path) {
    const int cell_size = 60;  // 更大的单元格尺寸
    const int rows = maze.size();
    const int cols = maze[0].size();

    // 创建图像
    Mat img(rows * cell_size, cols * cell_size, CV_8UC3, BACKGROUND_COLOR);

    // 绘制网格线（更细的线条）
    for (int i = 0; i <= rows; i++) {
        line(img, Point(0, i * cell_size), Point(cols * cell_size, i * cell_size),
            GRID_COLOR, 1, LINE_AA);
    }
    for (int j = 0; j <= cols; j++) {
        line(img, Point(j * cell_size, 0), Point(j * cell_size, rows * cell_size),
            GRID_COLOR, 1, LINE_AA);
    }

    // 绘制墙壁（带圆角效果）
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (maze[i][j] == 0) {
                rectangle(img,
                    Point(j * cell_size + 2, i * cell_size + 2),
                    Point((j + 1) * cell_size - 2, (i + 1) * cell_size - 2),
                    WALL_COLOR, FILLED, LINE_AA);
            }
        }
    }

    // 绘制路径（带渐变动画）
    if (!path.empty()) {
        // 绘制路径线（渐变色）
        for (size_t k = 1; k < path.size(); k++) {
            double ratio = static_cast<double>(k) / path.size();
            Scalar line_color = PATH_COLOR_START * (1 - ratio) + PATH_COLOR_END * ratio;

            Position prev = path[k - 1];
            Position curr = path[k];

            // 更平滑的线条
            line(img,
                Point(prev.y * cell_size + cell_size / 2, prev.x * cell_size + cell_size / 2),
                Point(curr.y * cell_size + cell_size / 2, curr.x * cell_size + cell_size / 2),
                line_color, 6, LINE_AA);
        }

        // 绘制路径点（动态大小）
        for (size_t k = 0; k < path.size(); k++) {
            Position p = path[k];
            double progress = static_cast<double>(k) / path.size();
            int radius = static_cast<int>(10 + 10 * sin(progress * CV_PI));

            Scalar point_color;
            if (k == 0) point_color = PATH_COLOR_START;
            else if (k == path.size() - 1) point_color = PATH_COLOR_END;
            else point_color = PATH_COLOR_MID * (1 - progress) + PATH_COLOR_END * progress;

            circle(img,
                Point(p.y * cell_size + cell_size / 2, p.x * cell_size + cell_size / 2),
                radius, point_color, FILLED, LINE_AA);

            // 添加白色边框使点更突出
            circle(img,
                Point(p.y * cell_size + cell_size / 2, p.x * cell_size + cell_size / 2),
                radius + 2, Scalar(255, 255, 255), 2, LINE_AA);
        }
    }

    // 添加专业的信息面板
    Rect info_panel(0, 0, cols * cell_size, 80);
    rectangle(img, info_panel, Scalar(255, 255, 255), FILLED);
    line(img, Point(0, 80), Point(cols * cell_size, 80), Scalar(200, 200, 200), 2, LINE_AA);

    // 添加标题
    putText(img, "MAZE SOLVER VISUALIZATION", Point(20, 30),
        FONT_HERSHEY_DUPLEX, 0.8, Scalar(50, 50, 50), 1, LINE_AA);

    // 添加路径信息
    string info_text = "Path Length: " + to_string(path.size());
    putText(img, info_text, Point(20, 60),
        FONT_HERSHEY_SIMPLEX, 0.6, Scalar(80, 80, 80), 1, LINE_AA);

    // 添加图例
    circle(img, Point(cols * cell_size - 150, 30), 8, PATH_COLOR_START, FILLED, LINE_AA);
    putText(img, "Start", Point(cols * cell_size - 130, 35),
        FONT_HERSHEY_SIMPLEX, 0.5, Scalar(80, 80, 80), 1, LINE_AA);

    circle(img, Point(cols * cell_size - 70, 30), 8, PATH_COLOR_END, FILLED, LINE_AA);
    putText(img, "End", Point(cols * cell_size - 50, 35),
        FONT_HERSHEY_SIMPLEX, 0.5, Scalar(80, 80, 80), 1, LINE_AA);

    // 显示图像（带平滑缩放）
    namedWindow("Maze Solver", WINDOW_NORMAL);
    resizeWindow("Maze Solver", 800, 600);
    imshow("Maze Solver", img);
    waitKey(0);
}
int main() {
    // 示例迷宫
    vector<vector<int>> maze = {
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 0, 1},
        {1, 1, 1, 1}
    };

    // 计时开始
    auto start_time = chrono::high_resolution_clock::now();

    auto path = solve_maze(maze);

    // 计时结束
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    if (path.empty()) {
        cout << "No solution found!" << endl;
    }
    else {
        cout << "Solution found in " << duration.count() << " ms" << endl;
        cout << "Path length: " << path.size() << endl;
    }

    visualize_maze(maze, path);
    return 0;
}