#ifndef WINDOW_TESSERACT_H
#define WINDOW_TESSERACT_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QStatusBar>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QImage>
#include <QTextEdit>
#include <QSplitter>
#include <QCheckBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

#include <iostream>

#include "tesseract/baseapi.h"

class Window_Tesseract : public QMainWindow
{
    Q_OBJECT
public:
    explicit Window_Tesseract(QWidget *parent = nullptr);
    ~Window_Tesseract();
    void showImage(QImage image);
    void showImage(QString path);
    void setAreasText(std::vector<QRect> &areas);
    
signals:
    void Signal_get_new_image(bool);

private:
    void createActions() ;
    void createUI() ;
    
    bool detectAreaChecked;
    QMenu *fileMenu;

    QToolBar *fileToolBar;

    QGraphicsScene *imageScene;
    QGraphicsView *imageView;

    QTextEdit *editor;

    QStatusBar *mainStatusBar;
    QLabel *mainStatusLabel;
    
    QAction *saveImageAsAction;
    QAction *saveTextAsAction;
    QAction *openImageAction;
    QAction *captureAction;
    QAction *ocrAction;
    QCheckBox *detectAreaCheckBox;
    
    QString currentImagePath;
    QGraphicsPixmapItem *currentImage;

    tesseract::TessBaseAPI *tesseractAPI;
    std::vector<QRect> areasText ;
    
private slots:
    void saveImageAs();
    void saveTextAs();
    void openImage();
    void extractText();
    void captureScreen();
};

#endif // WINDOW_TESSERACT_H
