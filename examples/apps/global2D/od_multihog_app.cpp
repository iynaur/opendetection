
/** \brief Example of the usage of HOG detector
   *
   * \author Kripasindhu Sarkar
   *
   */

#include "detectors/global2D/detection/ODHOGDetector.h"
#include "common/utils/ODFrameGenerator.h"

#include "common/pipeline/ObjectDetector.h"
#include "common/pipeline/ODDetection.h"


using namespace od;
using namespace std;

cv::Size sizesingle(640, 480);

int main(int argc, char *argv[])
{
  std::string trained_data_dir(argv[1]), input_video(argv[2]), output_video = "output.avi";
  if (argc > 3) output_video = argv[3];


  //get 3 detectors of different types
  vector<string> messages; messages.push_back("Original");
  vector<g2d::ODHOGDetector*> detectors;
  g2d::ODHOGDetector *detector1 = new g2d::ODHOGDetector; //
  messages.push_back("OpenCV Default People"); detectors.push_back(detector1);

  g2d::ODHOGDetector *detector2 = new g2d::ODHOGDetector; detector2->setSvmtype(g2d::ODHOGDetector::OD_DAIMLER_PEOPLE);
  messages.push_back("OpenCV Daimler People"); detectors.push_back(detector2);

  g2d::ODHOGDetector *detector3 = new g2d::ODHOGDetector(trained_data_dir);
  messages.push_back("Custom HOG from trained data"); detectors.push_back(detector3);

  //init all detectors
  for (int i = 0; i < detectors.size(); i++) detectors[i]->init();

  //get scenes
  od::ODFrameGenerator<od::ODSceneImage, od::GENERATOR_TYPE_DEVICE> frameGenerator(input_video);
  //od::ODFrameGenerator<od::ODSceneImage, od::GENERATOR_TYPE_FILE_LIST> frameGenerator(input_video);
  cv::VideoWriter videoWriter(output_video, CV_FOURCC('M','J','P','G'), 25, sizesingle * 2, true);


  //GUI
  cv::namedWindow("Overlay", cv::WINDOW_NORMAL);
  while(frameGenerator.isValid() && cv::waitKey(33) != 27)
  {
    od::ODSceneImage * scene = frameGenerator.getNextFrame();

    vector<cv::Mat> images_to_show;
    images_to_show.push_back(scene->getCVImage()); //push the first image

    //detect 3 times
    for (int i = 0; i < detectors.size(); i++)
    {
      ODDetections2D *detections =  detectors[i]->detectOmni(scene);
      if(detections->size() > 0)
        images_to_show.push_back(detections->renderMetainfo(*scene).getCVImage());
      else images_to_show.push_back(scene->getCVImage());
    }

    cv::Mat multiimage = makeCanvasMultiImages(images_to_show, sizesingle, messages);
    cv::imshow("Overlay", multiimage);
    videoWriter << multiimage;

    delete scene;
  }

  return 0;
}