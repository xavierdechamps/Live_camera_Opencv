# Live_camera_Opencv
This C++ program uses the built-in camera or any connected camera to perform image treatments. The image manipulations are performed by [OpenCV](https://github.com/opencv/opencv) and the interface is managed by Qt.

The implemented image manipulations are:
- Black and white + colour inversion
- Blur filters
  * Simple
  * Gaussian
  * Median
  * Bilateral
- Morphological filters
  * Erode + Dilate
  * Opening + Closing
  * Morphological gradient + Top hat + Black hat
  * Hit / Miss
- Threshold
  * Global threshold (binary, binary inverted, truncate, to zero, to zero inverted, Otsu, triangle method)
  * Adaptive threshold (mean and Gaussian)
- Histogram equalization (global + CLAHE)
- Edge detectors (Sobel derivatives, Laplacian and Canny edge detector)
- Face recognition
- Object 
