#include "dialog_choose_camera.h"

Dialog_choose_camera::Dialog_choose_camera(QList<QCameraInfo> cameras,QWidget *parent):    QDialog(parent){
    this->list_cameras = cameras;
    
    QList< QPushButton* > PB;
    QButtonGroup* group = new QButtonGroup(this);
    QGridLayout *grid = new QGridLayout;
    
    QLabel* label = new QLabel("Please choose one of the following cameras:");
    grid->addWidget(label, 0, 0);
    int i=0;
    foreach (const QCameraInfo &cameraInfo, cameras) {
        // Retrieve camera name
        QString info = cameraInfo.description() ;
        
        // Create new button with textValue == camera name
        QPushButton *button = new QPushButton(info, this);
        
        // Store button in an array
        PB << button;
        group->addButton(button,i);
        grid->addWidget(button, i+1, 0);
        i++;
    }

    connect(group, SIGNAL(buttonClicked(int)), this, SLOT(onClick_Choose_Camera(int)));
    
    setLayout(grid);
    setWindowTitle(tr("Camera control window"));
}

void Dialog_choose_camera::onClick_Choose_Camera(int val) {
    this->info_camera = this->list_cameras[val].description();
    emit this->Signal_camera_chosen( val ) ;
}

QString Dialog_choose_camera::get_info_camera() {
    return this->info_camera ;
}
