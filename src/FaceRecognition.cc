#include <iostream>
#include <chrono>
#include <string>
#include <vector>

#include <cstdio>

#include <opencv2/opencv.hpp>

#include "Network.hh"
#include "database.hh"

// class UserFaceModel {
//   std 
// };

constexpr int im_size = 256;
// constexpr int between_eyes = im_size*0.5;

cv::Point2f model_left_eye_center(im_size*0.25, im_size*0.2);
cv::Point2f model_right_eye_center(im_size*0.75, im_size*0.2);
cv::Point2f model_mouth_center(im_size*0.5, im_size*0.7);

void speek(std::string text) {
  std::stringstream ss;
  ss << "echo \"" << text << "\" | festival --tts";
  system(ss.str().c_str());
}

std::string ssystem(const char *command) {
  char tmpname [] = "/tmp/embeds.txt";//L_tmpnam];
  // std::tmpnam(tmpname);
  std::string scommand = command;
  std::string cmd = scommand + " | tee " + tmpname;
  std::system(cmd.c_str());
  std::ifstream file(tmpname, std::ios::in);
  std::string result;
  if (file) {
    while (!file.eof()) result.push_back(file.get());
        file.close();
  }

  // remove(tmpname);
  return result;
}

struct OneFaceModel {
  cv::Mat img;
  cv::Mat edges_cann;

  cv::Rect face;

  cv::Rect left_eye;
  cv::Rect right_eye;
  cv::Rect mouth;

  cv::Point left_eye_center;
  cv::Point right_eye_center;
  cv::Point mouth_center;
};

class FaceClassifier {
  private: std::map<std::string, OneFaceModel> face_base;
  private: float threshold = 0;

  public: void addPerson(OneFaceModel &face_model, std::string name) {
    this->face_base[name] = face_model;
  }

  public: std::map<std::string, float> getScores(OneFaceModel &face_model) {
    std::map<std::string, float> scores;

    for (auto &[k, v] : this->face_base) {
      auto score = this->getSingleScore(face_model, v);
      std::cout << k << ": " << score << std::endl;
      scores[k] = score;
    }

    return scores;
  }

  public: std::tuple<std::string, float> getBestScore(OneFaceModel &face_model) {
    auto scores_map = this->getScores(face_model);

    auto it = std::min_element(scores_map.begin(), scores_map.end(), [](auto &a, auto &b) {
      return std::get<1>(a) < std::get<1>(b);
    });

    if (it != scores_map.end())
      return *it;
  }

  public: std::vector<float> getEmbeddings(OneFaceModel &face_model) {
    cv::imwrite("/tmp/face.jpg", face_model.img);
    std::cout << "Firing up Facenet ... " << std::endl;
    // auto output = ssystem("cd node-facenet && ./node_modules/.bin/ts-node bin/embedding.ts /tmp/face.jpg && cd ..");
    auto output = ssystem("./get-embeddings.sh /tmp/face.jpg");
    auto it = output.rfind('\n');
    std::string embs = output.substr(it);
    std::cout << "Embeddings: " << output << std::endl;

    std::vector<float> embeddings;

    return embeddings;
  }

  public: bool empty() {
    return this->face_base.empty();
  }

  private: float getSingleScore(OneFaceModel &src, OneFaceModel &tmpl) {
    // cv::Mat dist_tr, labels;
    // cv::distanceTransform(tmpl.edges_cann, dist_tr, labels, cv::DIST_L2, 3);

    // double score = cv::sum(cv::mean(dist_tr, src.edges_cann))[0];
    cv::Mat diff, a, b;
    src.img.convertTo(a, CV_32FC1, 1/255.);
    tmpl.img.convertTo(b, CV_32FC1, 1/255.);
    diff = a-b;

    return cv::norm(diff);
  }

  public: void saveToFile(std::string filename) {
    cv::FileStorage fs(filename, cv::FileStorage::WRITE);

    fs << "size" << static_cast<int>(this->face_base.size());

    fs << "elements" << "[";

    for (auto &[key, value] : this->face_base) {
      fs << "{:" << "key" << key
                 << "edges_cann" << "[:";
                 for (int i = 0; i < value.edges_cann.rows; ++i)
                    for (int j = 0; j < value.edges_cann.cols; ++j)
                      if (value.edges_cann.at<uint8_t>(i, j) != 0)
                        fs << cv::Point(j, i);
                 fs << "]"
                 << "img" << "[:";
                 for (int i = 0; i < value.img.rows; ++i)
                    for (int j = 0; j < value.img.cols; ++j)
                      fs << value.img.at<uint8_t>(i, j);
                 fs << "]"
                 << "left_eye_center" << value.left_eye_center
                 << "right_eye_center" << value.right_eye_center
                 << "mouth_center" << value.mouth_center
      << "}";
    }
    fs << "]";

    fs.release();
  }

  public: void loadFromFile(std::string filename) {
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened()) {
      std::cout << "File " << filename << " not found" << std::endl;
      return;
    }

    int size;
    fs["size"] >> size;

    std::cout << "had " << size << " elements" << std::endl;

    cv::FileNode elements_node = fs["elements"];

    for (const auto &el : elements_node) {
      std::string key;
      OneFaceModel face_model;
      el["key"] >> key;
      // el["edges_cann"] >> face_model.edges_cann;
      cv::FileNode cann_node = el["edges_cann"];
      face_model.edges_cann = cv::Mat::zeros(im_size, im_size, CV_8UC1);
      for (const auto &ptn : cann_node) {
        cv::Point pt;
        ptn >> pt;
        face_model.edges_cann.at<uint8_t>(pt) = 255;
      }
      int i = 0;
      face_model.img = cv::Mat::zeros(im_size, im_size, CV_8UC1);
      cv::FileNode img_node = el["img"];
      for (const auto &ptn : img_node) 
      {
        int px;
        ptn >> px;
        face_model.img.at<uint8_t>(i/im_size, i%im_size) = px;
        i++;
      }
      el["left_eye_center"] >> face_model.left_eye_center; 
      el["right_eye_center"] >> face_model.right_eye_center; 
      el["mouth_center"] >> face_model.mouth_center; 
      std::cout << key << std::endl;
      this->face_base[key] = face_model;
    }

    fs.release();
  }
};

cv::Mat new_frame = cv::Mat::zeros(480 + im_size, std::max(640,im_size*2), CV_8UC3);

class FaceModelGenerator {
  public: FaceModelGenerator() {
    if(!face_cascade.load(face_cascade_name))
      throw std::runtime_error("Cascade file not found: face");
    if(!eye_cascade.load(eye_cascade_name))
      throw std::runtime_error("Cascade file not found: eye");
    if(!smile_cascade.load(smile_cascade_name))
      throw std::runtime_error("Cascade file not found: smile");
  }

  public: OneFaceModel getFaceModel(cv::Mat &frame) {
    cv::Mat gray_frame;
    // src.copyTo(frame);

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
      std::vector<cv::Rect> smiles;
      eye_cascade.detectMultiScale(gray_face_cut, eyes, 1.1, 2.0, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
      smile_cascade.detectMultiScale(gray_face_cut, smiles, 1.1, 2.0, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
      
      cv::rectangle(frame, face_rect, cv::Scalar(0, 255, 0), 2);

      if (eyes.size() != 2) {
        std::cout << "Are you even a human?!?! (eyes: " << eyes.size() << ")" << std::endl;
        throw std::runtime_error("No eyes");
      } else {
        cv::Rect isect = (eyes[0] & eyes[1]);
        if (isect.width != 0 || isect.height != 0) {
          throw std::runtime_error("Not enough eyes");
        }

        for (auto &eye : eyes) {
          cv::rectangle(face_roi, eye, cv::Scalar(255, 0, 0), 2);
        }

        auto it = std::max_element(smiles.begin(), smiles.end(), [](auto &a, auto &b) {
          auto y1 = a.height/2 + a.y;
          auto y2 = b.height/2 + b.y;

          return (y1 < y2);
        });

        if (it != smiles.end() && (*it & eyes[0]).area()==0 && (*it & eyes[1]).area()==0) {
          // Successfully got face
          cv::rectangle(face_roi, *it, cv::Scalar(255, 255, 0), 2);

          cv::Point2f e1_c(eyes[0].x+eyes[0].width/2, eyes[0].y+eyes[0].height/2);
          cv::Point2f e2_c(eyes[1].x+eyes[1].width/2, eyes[1].y+eyes[1].height/2);
          cv::Point2f m_c(it->x+it->width/2, it->y+it->height/2);

          if (e1_c.x > e2_c.x) {
            std::swap(eyes[0], eyes[1]);
            std::swap(e1_c, e2_c);
          }

          cv::line(face_roi, e1_c, e2_c, cv::Scalar(0, 255, 255), 2);
          cv::line(face_roi, e1_c, m_c, cv::Scalar(255, 0, 255), 2);
          cv::line(face_roi, e2_c, m_c, cv::Scalar(255, 0, 255), 2);

          // cv::Mat scale_eyes_aff = (cv::Mat_<float>(2, 3) << ;
          std::vector<cv::Point2f> src {
            e1_c, e2_c, m_c,
          };
          std::vector<cv::Point2f> dst {
            model_left_eye_center,
            model_right_eye_center,
            model_mouth_center,
          };

          cv::Mat cannonical_face;
          cv::Mat edges;
          cv::Canny(gray_face_cut, edges, 100, 128);

          cv::Mat affine_to_cann = cv::getAffineTransform(src, dst);
          cv::warpAffine(gray_face_cut, cannonical_face, affine_to_cann, cv::Size(im_size, im_size),
              CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS);
          cv::warpAffine(edges, edges, affine_to_cann, cv::Size(im_size, im_size),
              cv::INTER_NEAREST+CV_WARP_FILL_OUTLIERS);
          // std::cout << dst[0] << std::endl
          //           << dst[1] << std::endl
          //           << dst[2] << std::endl << std::endl;
          // std::cout << src[0] << std::endl
          //           << src[1] << std::endl
          //           << src[2] << std::endl << std::endl;
          // std::cout << affine_to_cann << std::endl;

          OneFaceModel face_model = {
            .img = cannonical_face,
            .edges_cann = edges,

            .face = face_rect,

            .left_eye = eyes[0],
            .right_eye = eyes[1],
            .mouth = *it,

            .left_eye_center = e1_c,
            .right_eye_center = e2_c,
            .mouth_center = m_c,
          };

          std::cout << "Got face" << std::endl;

          cv::Mat new_edges, new_cann_face;
          frame.copyTo(new_frame(cv::Rect(0,0,frame.cols,frame.rows)));
          // face_roi.copyTo(new_frame(cv::Rect(0, frame.rows, im_size, im_size)));
          cv::cvtColor(edges, new_edges, CV_GRAY2BGR);
          cv::cvtColor(cannonical_face, new_cann_face, CV_GRAY2BGR);
          new_edges.copyTo(new_frame(cv::Rect(im_size*0, frame.rows, im_size, im_size)));
          new_cann_face.copyTo(new_frame(cv::Rect(im_size*1, frame.rows, im_size, im_size)));

          frame = new_frame;

          // cv::imshow("Face", face_roi);
          // cv::imshow("FaceG", edges);
          // cv::imshow("FaceCan", cannonical_face);

          return face_model;
        }
        else
          throw std::runtime_error("Not mouth");
      }
    }
    else {
      cv::putText(frame, "No face", cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 0, 255), 2);
      throw std::runtime_error("No face");
    }

    std::cout << "Fuck" << std::endl;
    throw std::runtime_error("No face");
  }



  private: std::string face_cascade_name = "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml";
  private: std::string eye_cascade_name = "/usr/share/opencv/haarcascades/haarcascade_eye_tree_eyeglasses.xml";
  private: std::string smile_cascade_name = "/usr/share/opencv/haarcascades/haarcascade_smile.xml";

  private: cv::CascadeClassifier face_cascade;
  private: cv::CascadeClassifier eye_cascade;
  private: cv::CascadeClassifier smile_cascade;
};

#define Netw 1


int main(int argc, char **argv) {
#if Netw
  ConnectionToWebServer conn("10.1.3.46", "31285");
  // ConnectionToWebServer conn("127.0.0.1", "22");

  conn.launch();

  // for (int i = 0; i < 10; ++i) {
  //   sleep(1);
  //   conn.sendMsg("lol\n");
  // }
#endif

  FaceModelGenerator face_detector;

  DataBase db;

  cv::VideoCapture cap(0);
  if(!cap.isOpened())
    std::runtime_error("No camera");

  ::FaceClassifier face_classifier;
  face_classifier.loadFromFile("data/faces.yaml");

  cv::namedWindow("Cam", 1);
  // cv::namedWindow("Face", 1);
  // cv::namedWindow("FaceG", 1);

  int counter = 0;
  std::string names[] = {"Nick", "John", "Yaroslav", "Alex"};
  std::map<std::string, int> name_ids;
  name_ids[names[0]] = 1;
  name_ids[names[1]] = 2;
  name_ids[names[2]] = 3;
  name_ids[names[3]] = 4;

  std::chrono::system_clock::time_point last_report = std::chrono::system_clock::now() -  std::chrono::hours(1);
  std::string last_report_name = "";

  while(true)
  {
    cv::Mat frame;
    cap >> frame;

    try {
      auto face_model = face_detector.getFaceModel(frame);
      auto scores = face_classifier.getScores(face_model);

      cv::imshow("Cam", frame);
      auto key = cv::waitKey(30);
      if (key == 'a') {
        face_classifier.addPerson(face_model, names[counter++]);
        std::cout << "Added face: " << names[counter-1] << std::endl;
      } else if (key == 27)
        break;
      else /*if (key == ' ')*/ {
        // face_classifier.getEmbeddings(face_model);
        if (!face_classifier.empty()) {
          auto [name, score] = face_classifier.getBestScore(face_model);

          cv::putText(frame, name,
              cv::Point(face_model.face.x+face_model.face.width/2, face_model.face.y+face_model.face.height + 20),
              cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(0, 255, 0), 1);
          std::cout << "Name: " << name << "\nScore: " << score;
          // speek("Welcome, fellow customer, " + name + "!");

          auto now = std::chrono::system_clock::now();
          if (now - last_report > std::chrono::seconds(10) && last_report_name != name) {
            last_report = now;

            std::cout << "Informing UI ..." <<  std::endl;
            std::stringstream sstr;
            sstr << name;
            std::vector<int> wines = db.getRecomendations(name_ids[name]);
            for (auto i : wines)
              sstr << " " << i;
            sstr << "\n";
  #if Netw
            conn.sendMsg(sstr.str());
  #endif
          }
        }
        // std::cout << "Item scores: ";
        // for (auto &[name, score] : scores)
        //   std::cout << name << ": " << score << "; ";
        // std::cout << std::endl;
      }
    }
    catch(std::runtime_error &ex) {
      std::cout << "Face not detected: " << ex.what() << std::endl;
    }

    cv::imshow("Cam", frame);
    if (cv::waitKey(30) == 27) {
      break;
    }
  }

  face_classifier.saveToFile("data/faces.yaml");

  return 0;
}
