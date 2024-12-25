// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <opencv2/opencv.hpp>
#include <queue>
#include <vector>

using namespace cv;
using namespace std;



// 定义图节点结构体
struct Node {
    int x, y; // 坐标
    double cost; // 到起点的成本
};

// 比较函数，用于优先队列
struct CompareNode {
    bool operator()(const Node& a, const Node& b) {
        return a.cost > b.cost;
    }
};

// Dijkstra算法实现
void dijkstra(const Mat& image, Point start, vector<vector<double>>& dist) {
    int rows = image.rows, cols = image.cols;
    priority_queue<Node, vector<Node>, CompareNode> pq;
    dist.assign(rows, vector<double>(cols, DBL_MAX));
    dist[start.y][start.x] = 0;
    pq.push({ start.x, start.y, 0 });

    while (!pq.empty()) {
        Node current = pq.top();
        pq.pop();

        if (current.cost > dist[current.y][current.x]) continue;

        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (abs(dx) + abs(dy) == 2) continue; // 跳过对角线方向
                int newX = current.x + dx, newY = current.y + dy;
                if (newX >= 0 && newX < cols && newY >= 0 && newY < rows) {
                    double newCost = current.cost + abs(image.at<uchar>(newY, newX) - image.at<uchar>(current.y, current.x));
                    if (newCost < dist[newY][newX]) {
                        dist[newY][newX] = newCost;
                        pq.push({ newX, newY, newCost });
                    }
                }
            }
        }
    }
}

int main() {
    // 读取图像
    Mat image = imread("path/to/your/image.jpg", IMREAD_GRAYSCALE);
    if (image.empty()) {
        cout << "Could not open or find the image" << endl;
        return -1;
    }

    // 边缘检测
    Mat edges;
    Canny(image, edges, 50, 150);

    // 初始化距离矩阵
    vector<vector<double>> dist;
    Point start(50, 50); // 起始点

    // 应用Dijkstra算法
    dijkstra(edges, start, dist);

    // 输出或使用dist矩阵
    // 你可以根据dist矩阵来生成掩模或裁剪图像

    return 0;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件


