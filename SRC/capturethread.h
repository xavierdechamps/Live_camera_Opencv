#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QString>
#include <QThread>
#include <QMutex>

#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

using namespace std;

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    CaptureThread(int camera, QMutex *lock);
    ~CaptureThread() override;
    
    enum VideoSavingStatus {
                            STARTING,
                            STARTED,
                            STOPPING,
                            STOPPED
    };
    void setRunning(bool run) {running = run; }
    void startCalcFPS() {fps_calculating = true; }
    void setVideoSavingStatus(VideoSavingStatus status) {video_saving_status = status; }

protected:
    void run() override;

signals:
    void frameCaptured(cv::Mat *data);
    void fpsChanged(float fps);
    void videoSaved(QString name);

private:
    void calculateFPS(cv::VideoCapture &cap);
//    void startSavingVideo(cv::Mat &firstFrame);
    void stopSavingVideo();

private:
    bool running;
    int cameraID;
    QString videoPath;
    QMutex *data_lock;
    cv::Mat frame;

    // FPS calculating
    bool fps_calculating;
    float fps;

    // video saving
    int frame_width, frame_height;
    VideoSavingStatus video_saving_status;
    QString saved_video_name;
    cv::VideoWriter *video_writer;
};

#endif // CAPTURETHREAD_H
