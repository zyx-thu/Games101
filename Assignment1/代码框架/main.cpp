#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    Eigen::Matrix4f rotation;
    double radian = rotation_angle / 180 * acos(-1);
    rotation << cos(radian), -sin(radian), 0, 0, sin(radian), cos(radian), 0, 0,
        0, 0, 1, 0, 0, 0, 0, 1;

    model = rotation * model;
    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,//foY和宽高比
                                      float zNear, float zFar)
{
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    double radian = eye_fov / 180 * acos(-1);
    float t, b, l, r;
    t = zNear * tan(radian/2);
    b = -t;
    r = aspect_ratio * t;
    l = -r;
    Eigen::Matrix4f A;
    A << zNear, 0, 0, 0, 0, zNear, 0, 0, 0, 0, zNear + zFar, zNear * zFar,
        0, 0, -1, 0;
    Eigen::Matrix4f B;
    Eigen::Matrix4f B1, B2;
    B1 << 1,0,0,0,0,1,0,0,0,0,1,(zFar+zNear)/2,0,0,0,1;
    B2 << 1/r,0,0,0,0,1/t,0,0,0,0,2/(zNear-zFar),0,0,0,0,1;
    B = B2 * B1;
    projection = B * A * projection;
    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    return projection;
}

Eigen::Matrix4f get_rotation(Eigen::Vector3f axis, float angle)
{
    axis = axis.normalized();
    double radian = angle / 180 * acos(-1);
    Eigen::Matrix3f rotation3f;
    Eigen::Matrix3f rotationA;
    rotationA << cos(radian),0,0,0,cos(radian),0,0,0,cos(radian);
    Eigen::Matrix3f rotationB;
    rotationB << axis * axis.transpose();
    rotationB *= 1-cos(radian);
    Eigen::Matrix3f rotationC;
    rotationC << 0,-axis.z(),axis.y(),
                 axis.z(),0,-axis.x(),
                 -axis.y(),axis.x(),0;
    rotationC *= sin(radian);
    rotation3f = rotationA + rotationB +rotationC;
    Eigen::Matrix4f rotation = Eigen::Matrix4f::Identity();
    rotation.topLeftCorner<3,3>() = rotation3f;
    return rotation;
}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc >= 4) {
            filename = std::string(argv[3]);
        }

    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
