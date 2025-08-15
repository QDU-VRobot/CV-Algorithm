//#include <iostream>
//#include <opencv2/opencv.hpp>
//struct point_idx
//{
//    int n1, n2, n3;
//    point_idx(int a = 0, int b = 0, int c = 0) :n1(a), n2(b), n3(c) {}
//};
//std::vector<point_idx> delaunayTriangulation(std::vector<cv::KeyPoint> keypoints, cv::Mat& img) {
//    // 提取特征点
//    std::vector<cv::Point2f> cvPoints;
//    for (const auto& p : keypoints) {
//        cvPoints.push_back(p.pt);
//    }
//
//    // 创建用于存储三角剖分结果的容器
//    std::vector<cv::Vec6f> triangles;
//
//    // 使用 OpenCV 的德洛内三角剖分函数
//    cv::Subdiv2D subdiv(cv::Rect(0, 0, img.cols, img.rows));
//    for (const auto& p : cvPoints) {
//        subdiv.insert(p);
//    }
//    subdiv.getTriangleList(triangles);
//    std::vector<point_idx> points_idx;
//
//    // 在图像上绘制三角剖分结果
//    for (const auto& triangle : triangles) {
//        cv::Point2f pt1(triangle[0], triangle[1]);
//        cv::Point2f pt2(triangle[2], triangle[3]);
//        cv::Point2f pt3(triangle[4], triangle[5]);
//        cv::line(img, pt1, pt2, cv::Scalar(0, 255, 0), 1);
//        cv::line(img, pt2, pt3, cv::Scalar(0, 255, 0), 1);
//        cv::line(img, pt3, pt1, cv::Scalar(0, 255, 0), 1);
//        //根据opencv的结果回推连接顺序
//        bool is_pt1_already_matched = false;
//        bool is_pt2_already_matched = false;
//        bool is_pt3_already_matched = false;
//        for (int i = 0; i < cvPoints.size(); i++)
//        {
//            int a, b, c;
//            if (is_pt1_already_matched && is_pt2_already_matched && is_pt3_already_matched)
//            {
//                points_idx.push_back(point_idx(a, b, c));
//                break;
//            }
//            if (!is_pt1_already_matched && pt1 == cvPoints[i])
//            {
//                a = i;
//                is_pt1_already_matched = true;
//            }
//            else if (!is_pt2_already_matched && pt2 == cvPoints[i])
//            {
//                b = i;
//                is_pt2_already_matched = true;
//            }
//            else if (!is_pt3_already_matched && pt3 == cvPoints[i])
//            {
//                c = i;
//                is_pt3_already_matched = true;
//            }
//
//        }
//    }
//    return points_idx;
//}
//
//void sequentialConnection(std::vector<cv::KeyPoint> keypoints, std::vector<cv::DMatch> matches, std::vector<point_idx> points_idx, cv::Mat& img)
//{
//    for (int i = 0; i < points_idx.size(); i++) {
//        //获取第二帧图像上 与第一帧图像匹配的 特征点
//        cv::Point2f pt1 = keypoints.at(matches[points_idx[i].n1].trainIdx).pt;
//        cv::Point2f pt2 = keypoints.at(matches[points_idx[i].n2].trainIdx).pt;
//        cv::Point2f pt3 = keypoints.at(matches[points_idx[i].n3].trainIdx).pt;
//        //std::cout << pt1 << " ";
//        //std::cout << pt2 << " ";
//        //std::cout << pt3 << std::endl;
//        cv::line(img, pt1, pt2, cv::Scalar(0, 255, 0), 1);
//        cv::line(img, pt2, pt3, cv::Scalar(0, 255, 0), 1);
//        cv::line(img, pt3, pt1, cv::Scalar(0, 255, 0), 1);
//    }
//    //cv::imshow("", img);
//    //cv::waitKey(0);
//}
//int main() {
//
//    cv::Mat img1 = cv::imread("image7.jpeg", cv::IMREAD_GRAYSCALE);
//    cv::Mat img2 = cv::imread("image8.jpeg", cv::IMREAD_GRAYSCALE);
//    if (img1.empty() || img2.empty()) {
//        std::cout << "无法读取图像" << std::endl;
//        return -1;
//    }
//    cv::resize(img1, img1, cv::Size(300, 400));
//    cv::resize(img2, img2, cv::Size(300, 400));
//    //创建 ORB 对象并检测关键点和计算描述子
//    cv::Ptr<cv::FeatureDetector> detector = cv::ORB::create();
//    std::vector<cv::KeyPoint> keypoints1, keypoints2;
//    cv::Mat descriptors1, descriptors2;
//    detector->detectAndCompute(img1, cv::noArray(), keypoints1, descriptors1);
//    detector->detectAndCompute(img2, cv::noArray(), keypoints2, descriptors2);
//    //两帧图片特征匹配
//    cv::Ptr<cv::DescriptorMatcher> matcher = cv::BFMatcher::create(cv::NORM_HAMMING);
//    std::vector<cv::DMatch> matches;
//    matcher->match(descriptors1, descriptors2, matches);
//    std::cout << keypoints1.size() << std::endl;
//    std::cout << keypoints2.size() << std::endl;
//    //int minSize = keypoints1.size() < keypoints2.size() ? keypoints1.size() : keypoints2.size();
//    //std::cout << minSize << std::endl;
//    //for (int i = 0; i < matches.size(); i++)
//    //{
//    //   std::cout << matches[i].queryIdx << "->" << matches[i].trainIdx << std::endl;
//    //}
//    //system("pause");
//    cv::Mat img_matches;
//    cv::drawMatches(img1, keypoints1, img2, keypoints2, matches, img_matches);
//    
//    //进行德洛内三角剖分并绘制结果
//    cv::Mat img1_del = cv::Mat::zeros(img1.rows, img1.cols, CV_8UC3);
//    cv::Mat img2_del = cv::Mat::zeros(img2.rows, img2.cols, CV_8UC3);
//    std::vector<point_idx> points_idx = delaunayTriangulation(keypoints1, img1_del);
//    //for (int i = 0; i < points_idx.size(); i++)
//    //{
//    //    std::cout << points_idx[i].n1 << " ";
//    //    std::cout << points_idx[i].n2 << " ";
//    //    std::cout << points_idx[i].n3 << std::endl;
//    //}
//    sequentialConnection(keypoints2, matches, points_idx, img2_del);//img2根据img1的连接顺序连接
//    cv::imshow("Matches", img_matches);//orb结果
//    cv::imshow("img1_del", img1_del);//德洛内结果
//    cv::imshow("img2_del", img2_del);//德洛内结果
//    cv::waitKey(0);
//
//    return 0;
//}

#include <iostream>
#include <opencv2/opencv.hpp>

struct point_idx {
    int n1, n2, n3;
    point_idx(int a = -1, int b = -1, int c = -1) : n1(a), n2(b), n3(c) {}
};

std::vector<point_idx> delaunayTriangulation(std::vector<cv::KeyPoint> keypoints, cv::Mat& img) {
    // 提取特征点
    std::vector<cv::Point2f> cvPoints;
    for (const auto& p : keypoints) {
        cvPoints.push_back(p.pt);
    }

    // 创建用于存储三角剖分结果的容器
    std::vector<cv::Vec6f> triangles;

    // 使用 OpenCV 的德洛内三角剖分函数
    cv::Subdiv2D subdiv(cv::Rect(0, 0, img.cols, img.rows));
    for (const auto& p : cvPoints) {
        subdiv.insert(p);
    }
    subdiv.getTriangleList(triangles);
    std::vector<point_idx> points_idx;

    const float eps = 1e-3f;

    // 在图像上绘制三角剖分结果
    for (const auto& triangle : triangles) {
        cv::Point2f pt1(triangle[0], triangle[1]);
        cv::Point2f pt2(triangle[2], triangle[3]);
        cv::Point2f pt3(triangle[4], triangle[5]);

        // 过滤掉越界的三角形
        if (pt1.x < 0 || pt1.y < 0 || pt1.x >= img.cols || pt1.y >= img.rows ||
            pt2.x < 0 || pt2.y < 0 || pt2.x >= img.cols || pt2.y >= img.rows ||
            pt3.x < 0 || pt3.y < 0 || pt3.x >= img.cols || pt3.y >= img.rows) {
            continue;
        }

        cv::line(img, pt1, pt2, cv::Scalar(255, 0, 0), 1);
        cv::line(img, pt2, pt3, cv::Scalar(255, 0, 0), 1);
        cv::line(img, pt3, pt1, cv::Scalar(255, 0, 0), 1);

        int a = -1, b = -1, c = -1;
        bool is_pt1_matched = false;
        bool is_pt2_matched = false;
        bool is_pt3_matched = false;

        for (int i = 0; i < cvPoints.size(); i++) {
            if (!is_pt1_matched && cv::norm(pt1 - cvPoints[i]) < eps) {
                a = i;
                is_pt1_matched = true;
            }
            else if (!is_pt2_matched && cv::norm(pt2 - cvPoints[i]) < eps) {
                b = i;
                is_pt2_matched = true;
            }
            else if (!is_pt3_matched && cv::norm(pt3 - cvPoints[i]) < eps) {
                c = i;
                is_pt3_matched = true;
            }
        }

        if (a != -1 && b != -1 && c != -1) {
            points_idx.push_back(point_idx(a, b, c));
        }
    }
    return points_idx;
}

void sequentialConnection(std::vector<cv::KeyPoint> keypoints, std::vector<cv::DMatch> matches, std::vector<point_idx> points_idx, cv::Mat& img) {
    for (const auto& tri : points_idx) {
        cv::Point2f pt1 = keypoints.at(matches[tri.n1].trainIdx).pt;
        cv::Point2f pt2 = keypoints.at(matches[tri.n2].trainIdx).pt;
        cv::Point2f pt3 = keypoints.at(matches[tri.n3].trainIdx).pt;

        cv::line(img, pt1, pt2, cv::Scalar(0, 255, 0), 1);
        cv::line(img, pt2, pt3, cv::Scalar(0, 255, 0), 1);
        cv::line(img, pt3, pt1, cv::Scalar(0, 255, 0), 1);
    }
}

int main() {
    cv::Mat img1 = cv::imread("image7.jpeg", cv::IMREAD_GRAYSCALE);
    cv::Mat img2 = cv::imread("image8.jpeg", cv::IMREAD_GRAYSCALE);
    if (img1.empty() || img2.empty()) {
        std::cout << "无法读取图像" << std::endl;
        return -1;
    }

    cv::resize(img1, img1, cv::Size(300, 400));
    cv::resize(img2, img2, cv::Size(300, 400));

    // ORB 检测
    cv::Ptr<cv::FeatureDetector> detector = cv::ORB::create();
    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat descriptors1, descriptors2;
    detector->detectAndCompute(img1, cv::noArray(), keypoints1, descriptors1);
    detector->detectAndCompute(img2, cv::noArray(), keypoints2, descriptors2);

    // BFMatcher 匹配
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::BFMatcher::create(cv::NORM_HAMMING);
    std::vector<cv::DMatch> matches;
    matcher->match(descriptors1, descriptors2, matches);

    // 输出关键点和匹配数量
    std::cout << "Image1 keypoints: " << keypoints1.size() << std::endl;
    std::cout << "Image2 keypoints: " << keypoints2.size() << std::endl;
    std::cout << "Matches found: " << matches.size() << std::endl;

    cv::Mat img_matches;
    cv::drawMatches(img1, keypoints1, img2, keypoints2, matches, img_matches);

    // 德洛内三角剖分
    cv::Mat img1_del = cv::Mat::zeros(img1.rows, img1.cols, CV_8UC3);
    cv::Mat img2_del = cv::Mat::zeros(img2.rows, img2.cols, CV_8UC3);
    std::vector<point_idx> points_idx = delaunayTriangulation(keypoints1, img1_del);

    sequentialConnection(keypoints2, matches, points_idx, img2_del);

    cv::imshow("Matches", img_matches);
    cv::imshow("img1_del", img1_del);
    cv::imshow("img2_del", img2_del);
    cv::waitKey(0);

    return 0;
}
