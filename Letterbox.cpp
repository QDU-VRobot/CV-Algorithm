#include <opencv2/opencv.hpp>

/**
 * @brief 对图像进行Letterbox预处理
 * 
 * @param src 原始图像
 * @param dst [输出] 处理后的图像
 * @param new_shape 目标形状（模型输入尺寸）
 * @param ratio [输出] 记录实际使用的缩放比例
 * @param dw [输出] 记录宽度方向的单边padding
 * @param dh [输出] 记录高度方向的单边padding
 * @param color 填充区域的颜色
 * @param scaleup 是否允许上采样
 */
inline void letterbox(const cv::Mat& src, cv::Mat& dst, const cv::Size& new_shape, float& ratio, int& dw, int& dh,
                const cv::Scalar& color = cv::Scalar(114, 114, 114), bool scaleup = true) {
    // 1. 计算缩放比例
    ratio = std::min((float)new_shape.width / src.cols, (float)new_shape.height / src.rows);
    if (!scaleup) {
        ratio = std::min(ratio, 1.0f);
    }

    // 2. 计算缩放后的尺寸
    int new_w = static_cast<int>(round(src.cols * ratio));
    int new_h = static_cast<int>(round(src.rows * ratio));

    // 3. 计算需要填充的padding
    dw = (new_shape.width - new_w) / 2;
    dh = (new_shape.height - new_h) / 2;

    // 4 创建一个最终尺寸的画布，并填充颜色
    dst.create(new_shape, src.type());
    dst.setTo(color);

    cv::Rect roi(dw, dh, new_w, new_h);

    cv::Mat roi_target = dst(roi);
    cv::resize(src, roi_target, roi_target.size(), 0, 0, cv::INTER_LINEAR);
}

/**
 * @brief 将letterbox处理后图像上的坐标反算回原图坐标
 * 
 * @param box_on_letterboxed 在letterbox图像上的检测框 (cv::Rect)
 * @param ratio letterbox处理时使用的缩放比例
 * @param dw letterbox处理时宽度方向的padding
 * @param dh letterbox处理时高度方向的padding
 * @param original_shape 原始图像的尺寸 cv::Size(width, height)
 * @return cv::Rect 在原始图像上的对应检测框
 */
inline cv::Rect map_coordinates_back(const cv::Rect& box, float ratio, int dw, int dh, const cv::Size& original_shape) {
    // orig_coord = (letterbox_coord - padding) / ratio
    int x1 = static_cast<int>((box.x - dw) / ratio);
    int y1 = static_cast<int>((box.y - dh) / ratio);
    int x2 = static_cast<int>((box.x + box.width - dw) / ratio);
    int y2 = static_cast<int>((box.y + box.height - dh) / ratio);

    // 裁剪坐标
    x1 = std::max(0, std::min(x1, original_shape.width));
    y1 = std::max(0, std::min(y1, original_shape.height));
    x2 = std::max(0, std::min(x2, original_shape.width));
    y2 = std::max(0, std::min(y2, original_shape.height));

    return cv::Rect(x1, y1, x2 - x1, y2 - y1);
}

int main() {
    // 测试图像
    cv::Mat image = cv::Mat::zeros(cv::Size(600, 400), CV_8UC3);
    cv::Rect object(150, 100, 200, 150);
    cv::rectangle(image, object, cv::Scalar(255, 0, 0), -1);

    // Letterbox
    cv::Size model_input_size(640, 640);
    
    // 接收letterbox结果
    cv::Mat letterboxed_image;
    float ratio;
    int dw, dh;

    // 信息通过引用被填充
    letterbox(image, letterboxed_image, model_input_size, ratio, dw, dh);

    // std::cout << "图像已处理为 " << model_input_size.width << "x" << model_input_size.height << " (Letterbox)。" << std::endl;
    // std::cout << "  - 缩放比例 (ratio): " << ratio << std::endl;
    // std::cout << "  - 宽度填充 (dw): " << dw << " pixels" << std::endl;
    // std::cout << "  - 高度填充 (dh): " << dh << " pixels" << std::endl;

    // 模拟在Letterbox图像上进行目标检测
    int x_prime = static_cast<int>(object.x * ratio + dw);
    int y_prime = static_cast<int>(object.y * ratio + dh);
    int w_prime = static_cast<int>(object.width * ratio);
    int h_prime = static_cast<int>(object.height * ratio);
    cv::Rect box(x_prime, y_prime, w_prime, h_prime);
    
    // std::cout << "Letterbox图像上检测到物体，其坐标为: " << box << std::endl;
    cv::rectangle(letterboxed_image, box, cv::Scalar(0, 0, 255), 2);

    // 将检测框坐标反算回原始图像
    // 将之前获取的 ratio, dw, dh 等信息传入
    cv::Rect box_original = map_coordinates_back(box, ratio, dw, dh, image.size());

    // std::cout << "将检测框坐标反算回原始图像，得到坐标: " << box_original << std::endl;
    // std::cout << "原始物体坐标为: " << object << " (用于对比)" << std::endl;

    cv::rectangle(image, box_original, cv::Scalar(0, 255, 0), 2);

    // 显示结果
    cv::imshow("Original Image", image);
    cv::imshow("Letterboxed Image", letterboxed_image);
    cv::waitKey(0);

    return 0;
}
