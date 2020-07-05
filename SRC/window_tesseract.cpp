/*
 * Copyright (C) 2019-2020 Xavier Dechamps
 *
 * PURPOSE
 *   Management of the Qt window that deals with text detection and decryption. The button "Capture Screen"
 *   picks up an image from the camera stream and shows it on the left part of the window. The button "OCR"
 *   proceeds with the text decryption through the external library Tesseract. The checkbox "Detect Text Areas"
 *   launches the OpenCV text detection to isolate words inside the whole image in order to ease the decryption.
 *   The decrypted text is printed on the right part of the window. 
*/

#include "window_tesseract.h"

/**
 * @brief Window_Tesseract::Window_Tesseract
 * @param parent
 * 
 * Constructor of the class Window_Tesseract. Set the appearance of the widget and create connections between
 * the buttons/checkbox and the functionnalities
 */
Window_Tesseract::Window_Tesseract(QWidget *parent) : QMainWindow(parent)
  , currentImage(nullptr)
  , tesseractAPI(nullptr)
{   
    createUI();
    createActions();
}

/**
 * @brief Window_Tesseract::~Window_Tesseract
 * 
 * Destructor of the class Window_Tesseract. It ensures the correct deletion of the object of Tesseract.
 */
Window_Tesseract::~Window_Tesseract() {
    // Destroy used object and release memory
    if(tesseractAPI != nullptr) {
        tesseractAPI->End();
        delete tesseractAPI;
    }
}

/**
 * @brief Window_Tesseract::createUI
 * 
 * Set the appearance of the widget.
 */
void Window_Tesseract::createUI(){
    this->resize(800, 600);
    // setup menubar
    fileMenu = menuBar()->addMenu("&File");

    // setup toolbar
    fileToolBar = addToolBar("File");

    // main area
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    imageScene = new QGraphicsScene(this);
    imageView = new QGraphicsView(imageScene);
    splitter->addWidget(imageView);

    editor = new QTextEdit(this);
    splitter->addWidget(editor);

    QList<int> sizes = {400, 400};
    splitter->setSizes(sizes);

    setCentralWidget(splitter);

    // setup status bar
    mainStatusBar = statusBar();
    mainStatusLabel = new QLabel(mainStatusBar);
    mainStatusBar->addPermanentWidget(mainStatusLabel);
    mainStatusLabel->setText("Application Information will be here!");
}

/**
 * @brief Window_Tesseract::createActions
 * 
 * Create the actions linked to the buttons/checkbox
 */
void Window_Tesseract::createActions(){
    this->saveImageAsAction = new QAction("Save &Image as", this);
    this->fileMenu->addAction(this->saveImageAsAction);
    
    this->saveTextAsAction = new QAction("Save &Text as", this);
    this->fileMenu->addAction(this->saveTextAsAction);
    
    this->openImageAction = new QAction("Open Image", this);
    this->fileMenu->addAction(this->openImageAction);
    
    // add actions to toolbars
    this->captureAction = new QAction("Capture Screen", this);
    this->fileToolBar->addAction(this->captureAction);
    
    this->ocrAction = new QAction("OCR", this);
    this->fileToolBar->addAction(this->ocrAction);
    
    this->detectAreaCheckBox = new QCheckBox("Detect Text Areas", this);
    this->detectAreaCheckBox->setChecked(false); 
    this->detectAreaChecked = false;
    this->fileToolBar->addWidget(this->detectAreaCheckBox);

    // connect the signals and slots
    connect(this->saveImageAsAction, SIGNAL(triggered(bool)), this, SLOT(saveImageAs()) );
    connect(this->saveTextAsAction , SIGNAL(triggered(bool)), this, SLOT(saveTextAs()) );
    connect(this->openImageAction  , SIGNAL(triggered(bool)), this, SLOT(openImage()) );
    connect(this->ocrAction        , SIGNAL(triggered(bool)), this, SLOT(extractText()) );
    connect(this->captureAction    , SIGNAL(triggered(bool)), this, SLOT(captureScreen()) );
}

void Window_Tesseract::openImage()
{
    QFileDialog dialog(this);
    dialog.setWindowTitle("Open Image");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
    QStringList filePaths;
    if (dialog.exec()) {
        filePaths = dialog.selectedFiles();
        showImage(filePaths.at(0));
    }
}

/**
 * @brief Window_Tesseract::showImage
 * @param path: QString containing the path+name of the image to display on the left part of the window
 * 
 * Displays a local image on the left part of the window
 */
void Window_Tesseract::showImage(QString path)
{
    QPixmap image(path);
    showImage(image.toImage());
    currentImagePath = path;
    QString status = QString("%1, %2x%3, %4 Bytes").arg(path).arg(image.width())
        .arg(image.height()).arg(QFile(path).size());
    mainStatusLabel->setText(status);
}

/**
 * @brief Window_Tesseract::showImage
 * @param image2show: QImage to display on the left part of the window
 * 
 * Receives a QImage as parameter and displays it on the left part of the window
 */
void Window_Tesseract::showImage(QImage image2show) {
    QPixmap piximage = QPixmap::fromImage(image2show);
    
    imageScene->clear();
    imageView->resetMatrix();
    currentImage = imageScene->addPixmap(piximage);
    imageScene->update();
    imageView->setSceneRect(piximage.rect());
    
    QString status = QString("Received a new image of size %2x%3 pixels").arg(piximage.width()).arg(piximage.height());
    mainStatusLabel->setText(status);
}

/**
 * @brief Window_Tesseract::setAreasText
 * @param areas: vector of QRect.
 * 
 * Receives the vector of rectangles that defines the areas containing text.
 */
void Window_Tesseract::setAreasText(std::vector<QRect> &areas) {
    this->areasText = areas;
}

/**
 * @brief Window_Tesseract::saveImageAs
 * 
 * Saves the temporary image to a local file. It opens a Qt dialog window to set the path+name of the file.
 */
void Window_Tesseract::saveImageAs()
{
    if (currentImage == nullptr) {
        QMessageBox::information(this, "Information", "Noting to save.");
        return;
    }
    QFileDialog dialog(this);
    dialog.setWindowTitle("Save Image As ...");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
        if(QRegExp(".+\\.(png|bmp|jpg)").exactMatch(fileNames.at(0))) {
            currentImage->pixmap().save(fileNames.at(0));
            QString status = QString("Image saved under ")+fileNames.at(0);
            mainStatusLabel->setText(status);
        }
        else {
            QMessageBox::information(this, "Error", "Save error: bad format or filename.");
        }
    }
}

/**
 * @brief Window_Tesseract::saveTextAs
 * 
 * Saves the decoded text to a local file. It opens a Qt dialog window to set the path+name of the file.
 */
void Window_Tesseract::saveTextAs()
{
    QFileDialog dialog(this);
    dialog.setWindowTitle("Save Text As ...");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Text files (*.txt)"));
    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
        if(QRegExp(".+\\.(txt)").exactMatch(fileNames.at(0))) {
            QFile file(fileNames.at(0));
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QMessageBox::information(this, "Error", "Can't save text.");
                return;
            }
            QTextStream out(&file);
            out << editor->toPlainText() << "\n";
            QString status = QString("Text saved under ")+fileNames.at(0);
            mainStatusLabel->setText(status);
        } else {
            QMessageBox::information(this, "Error", "Save error: bad format or filename.");
        }
    }
}

/**
 * @brief Window_Tesseract::extractText
 * 
 * The core of the text decryption. It initializes the Tesseract object and feeds it with the image / parameters
 * to decode the text. If text areas were previously detected (checkbox), then these are provided to the 
 * Tesseract object to ease the decryption of text in an image.
 * 
 */
void Window_Tesseract::extractText()
{
    QString status = QString("Extracting the text...");
    mainStatusLabel->setText(status);
    
    if (currentImage == nullptr) {
        QMessageBox::information(this, "Information", "No opened image.");
        return;
    }
    // The locale must be "C" for the Tesseract library to work. Save the current one and switch it to "C".
    char *old_ctype = strdup(setlocale(LC_ALL, nullptr));
    setlocale(LC_ALL, "C");
    
    // Initialize the Tesseract object.
    if (tesseractAPI == nullptr) {
        tesseractAPI = new tesseract::TessBaseAPI();
        // Initialize tesseract-ocr with English, with specifying tessdata path
        if (tesseractAPI->Init(TESSERACT_DATA, "eng")) {
            QMessageBox::information(this, "Error", "Could not initialize tesseract.");
            return;
        }
    }

    QPixmap pixmap = currentImage->pixmap();
    QImage image = pixmap.toImage();
    image = image.convertToFormat(QImage::Format_RGB888);

    // Feed the Tesseract object with the data from the image
    tesseractAPI->SetImage(image.bits(), image.width(), image.height(), 3, image.bytesPerLine());
    
    // Decode the text in the image. First case is when text areas were previously detected (checkbox)
    if (detectAreaCheckBox->checkState() == Qt::Checked && !this->areasText.empty()) {
        editor->setPlainText("");
        for(QRect &rect : this->areasText) {
            tesseractAPI->SetRectangle(rect.y(), rect.x(), rect.height(), rect.width());
            char *outText = tesseractAPI->GetUTF8Text();
            editor->setPlainText(editor->toPlainText() + outText);
            delete [] outText;
        }
    }
    else { // Case when no text area has been detected
        char *outText = tesseractAPI->GetUTF8Text();
        editor->setPlainText(outText);
        delete [] outText;
    }

    // Switch back to the original locale
    setlocale(LC_ALL, old_ctype);
    free(old_ctype);
    
    status = QString("Done extracting the text");
    mainStatusLabel->setText(status);
}

/**
 * @brief Window_Tesseract::captureScreen
 * 
 * Send a signal to the external world to receive a new image (to be set by the function showImage)
 */
void Window_Tesseract::captureScreen(){
    emit this->Signal_get_new_image(detectAreaCheckBox->isChecked());
}
