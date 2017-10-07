#include <iostream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>


int main(int argc, char **argv) {
  std::string face_cascade_name = "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml";
  std::string eye_cascade_name = "/usr/share/opencv/haarcascades/haarcascade_eye_tree_eyeglasses.xml";
  // std::string eye_cascade_name = "/usr/share/opencv/haarcascades/haarcascade_lefteye_2splits.xml";

  cv::CascadeClassifier face_cascade;
  cv::CascadeClassifier eye_cascade;

  if(!face_cascade.load(face_cascade_name))
    throw std::runtime_error("Cascade file not found: face");
  if(!eye_cascade.load(eye_cascade_name))
    throw std::runtime_error("Cascade file not found: eye");

  cv::VideoCapture cap(0);
  if(!cap.isOpened())
    std::runtime_error("No camera");

  cv::namedWindow("Cam", 1);
  cv::namedWindow("Face", 1);
  while(true)
  {
    cv::Mat frame, gray_frame;
    cap >> frame;

    cv::cvtColor(frame, gray_frame, CV_BGR2GRAY);
    cv::equalizeHist(gray_frame, gray_frame);

    std::vector<cv::Rect> faces;

    face_cascade.detectMultiScale(gray_frame, faces, 1.1, 2.0, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));

    if (faces.size()) {
      auto &face_rect = faces[0];
      cv::Mat face_roi = frame(face_rect);
      cv::Mat gray_face_cut;
      cv::cvtColor(face_roi, gray_face_cut, CV_BGR2GRAY);
      cv::equalizeHist(gray_face_cut, gray_face_cut);

      std::vector<cv::Rect> eyes;
      eye_cascade.detectMultiScale(gray_face_cut, eyes, 1.1, 2.0, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
      
      cv::rectangle(frame, face_rect, cv::Scalar(0, 255, 0), 2);

      // for (auto &eye : eyes) {
      //   cv::rectangle(face_roi, eye, cv::Scalar(255, 0, 0), 2);
      // }


      if (eyes.size() != 2) {
        std::cout << "Are you even a human?!?! (eyes: " << eyes.size() << ")" << std::endl;
      } else {
        cv::Rect isect = (eyes[0] & eyes[1]);
        if (isect.width != 0 || isect.height != 0) {
          continue;
        }

        for (auto &eye : eyes) {
          cv::rectangle(face_roi, eye, cv::Scalar(255, 0, 0), 2);
        }
        cv::rectangle(face_roi, isect, cv::Scalar(255, 255, 0), 2);
        cv::imshow("Face", face_roi);
      }
    }
    else
      cv::putText(frame, "No face", cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 0, 255), 2);

    cv::imshow("Cam", frame);

    if (cv::waitKey(30) == 27) {
      // std::cout << q << std::endl;
      break;
    }
  }

  return 0;
}
