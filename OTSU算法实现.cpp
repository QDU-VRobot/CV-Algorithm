#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;

/**
 * @brief 改进版Otsu阈值分割算法，自动计算最佳二值化阈值
 * @param input 输入图像（必须是单通道8位灰度图 CV_8UC1）
 * @param printThresh 是否打印最终阈值（默认false）
 * @return 最佳阈值（0~255）；若输入非法，返回-1
 */
int thresh_otsu(const cv::Mat& input, bool printThresh = false)
{
    // 1. 输入合法性检查（避免后续错误）
    if (input.empty())
    {
        cerr << "[ERROR] 输入图像为空！" << endl;
        return -1;
    }
    if (input.type() != CV_8UC1)
    {
        cerr << "[ERROR] 输入必须是单通道8位灰度图（CV_8UC1）！" << endl;
        return -1;
    }

    // 2. 统计灰度直方图 + 全局总灰度值（避免重复计算）
    const int GRAY_LEVEL = 256;
    long long histogram[GRAY_LEVEL] = { 0 }; // 用long long防溢出（支持大图像）
    long long globalGraySum = 0;             // 全局总灰度值（所有像素灰度之和）
    const int imgSize = input.rows * input.cols;

    // 高效遍历图像（行指针 + 连续内存判断）
    if (input.isContinuous())
    {
        // 图像内存连续，可按1D数组遍历（更快）
        const uchar* data = input.data;
        for (int i = 0; i < imgSize; ++i)
        {
            int gray = data[i];
            histogram[gray]++;
            globalGraySum += gray;
        }
    }
    else
    {
        // 内存不连续，按行遍历
        for (int row = 0; row < input.rows; ++row)
        {
            const uchar* data = input.ptr<uchar>(row);
            for (int col = 0; col < input.cols; ++col)
            {
                int gray = data[col];
                histogram[gray]++;
                globalGraySum += gray;
            }
        }
    }

    // 3. 遍历阈值计算类间方差（优化：用累加代替重复遍历直方图）
    long long fgPixelCount = 0;  // 前景像素数（灰度 ≤ 当前阈值）
    long long fgGraySum = 0;     // 前景总灰度值（灰度 ≤ 当前阈值）
    double maxInterVariance = 0; // 最大类间方差
    int bestThresh = 0;          // 最佳阈值

    for (int thresh = 0; thresh < GRAY_LEVEL; ++thresh)
    {
        // 累加当前灰度级到前景（避免每次遍历0~thresh）
        fgPixelCount += histogram[thresh];
        fgGraySum += histogram[thresh] * thresh;

        // 跳过无意义阈值（前景/背景像素数为0，无法分割）
        if (fgPixelCount == 0 || fgPixelCount == imgSize)
        {
            continue;
        }

        // 计算前景/背景参数（用全局值减前景值，避免二次遍历直方图）
        long long bgPixelCount = imgSize - fgPixelCount; // 背景像素数
        long long bgGraySum = globalGraySum - fgGraySum; // 背景总灰度值

        // 前景/背景均值（用double确保精度）
        double fgMean = static_cast<double>(fgGraySum) / fgPixelCount;
        double bgMean = static_cast<double>(bgGraySum) / bgPixelCount;

        // 前景/背景概率
        double fgProb = static_cast<double>(fgPixelCount) / imgSize;
        double bgProb = static_cast<double>(bgPixelCount) / imgSize;

        // 计算类间方差（Otsu核心公式）
        double interVariance = fgProb * bgProb * pow(bgMean - fgMean, 2);

        // 更新最大类间方差和最佳阈值
        if (interVariance > maxInterVariance)
        {
            maxInterVariance = interVariance;
            bestThresh = thresh;
        }
    }

    // 可选：打印最佳阈值
    if (printThresh)
    {
        cout << "[INFO] Otsu最佳阈值：" << bestThresh << endl;
    }

    return bestThresh;
}



int main()
{
    // 读取灰度图
    cv::Mat input = cv::imread("test_gray.jpg", cv::IMREAD_GRAYSCALE);
    if (input.empty())
    {
        cerr << "[ERROR] 无法读取图像！" << endl;
        return -1;
    }

    // 计算Otsu阈值（打印阈值）
    int bestThresh = thresh_otsu(input, true);
    if (bestThresh == -1)
    {
        return -1;
    }

    // 用最佳阈值二值化
    cv::Mat output;
    cv::threshold(input, output, bestThresh, 255, cv::THRESH_BINARY);

    // 显示结果
    cv::imshow("输入灰度图", input);
    cv::imshow("Otsu二值化结果", output);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
