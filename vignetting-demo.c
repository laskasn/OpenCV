#include<iostream>
using namespace std;
#include<math.h>
#include<cv.h>
#include<cxcore.h>
#include<highgui.h>
#define P 3.14159
#define MAX_ANGLE P/9 //from center of image to one corner (whichever one)
// P/9 means 180degrees/9 = 20 degrees , which is half the angle of the lens.
// The lens angle is 20*2 = 40 degrees in this case.

double dist (CvPoint a,CvPoint b){
    return sqrt(pow((double)(a.x-b.x),2)+pow((double)(a.y-b.y),2));
}

void mechanicalVignetting(int pixels_falloff){
   int image_width = 400,image_height = 300;
   double radius = (double)(image_width/2)*95/100;
   CvPoint image_center = cvPoint(image_width/2,image_height/2);
   double max_image_rad = sqrt(pow((double)image_width/2,2)+pow((double)image_height/2,2));
   CvSize image_size = cvSize(image_width,image_height);
   IplImage * eikona = cvCreateImage(image_size,IPL_DEPTH_64F,1);
   cvSet(eikona,cvScalar(1));
   double distance;
   for(int i=0;i<eikona->height;i++){
     for(int j=0;j<eikona->width;j++){
       distance = dist(image_center,cvPoint(j,i));
       if(distance>radius){
         if(distance>radius+pixels_falloff)
            cvSet2D(eikona,i,j,cvScalar(0));
         else
            cvSet2D(eikona,i,j,cvScalar(1-pow((double)((distance-radius)/pixels_falloff),2)));
       }
     }
   }
   cvNamedWindow("Mechanical Vignetting");
   cvShowImage("Mechanical Vignetting",eikona);
   //cvWaitKey(0);
   //cvDestroyWindow("Mechanical Vignetting");
   cvReleaseImage(&eikona);
} 


IplImage * naturalVignetting(int image_width = 400, int image_height = 300){
   CvPoint image_center = cvPoint(image_width/2,image_height/2);
   double max_image_rad = sqrt(pow((double)image_width/2,2)+pow((double)image_height/2,2));
   CvSize image_size = cvSize(image_width,image_height);
   IplImage * eikona = cvCreateImage(image_size,IPL_DEPTH_64F,1);
   cvSet(eikona,cvScalar(1));
   for(int i=0;i<eikona->height;i++)
      for(int j=0;j<eikona->width;j++)
        cvSet2D(eikona,i,j,cvScalar(pow(cos((dist(image_center,cvPoint(j,i))/max_image_rad)*MAX_ANGLE),4)));
   cvNamedWindow("Natural Vignetting (intensity)");
   cvShowImage("Natural Vignetting (intensity)",eikona);
   //cvWaitKey(0);
   //cvDestroyWindow("Natural Vignetting");
   return eikona;
}


void artificial_vignetting(char * filename){
   IplImage * bgr = cvLoadImage(filename);
   cvNamedWindow("Original Image");
   cvShowImage("Original Image",bgr);
   //cvWaitKey(0);
   CvScalar value;
   IplImage * Lab = cvCreateImage(cvGetSize(bgr),IPL_DEPTH_8U,3);
   IplImage * reference = naturalVignetting(Lab->width,Lab->height);
   cvCvtColor(bgr,Lab,CV_BGR2Lab);
   for(int i=0;i<Lab->height;i++){
     for(int j=0;j<Lab->width;j++){
       value = cvGet2D(Lab,i,j);
       value.val[0] *= cvGet2D(reference,i,j).val[0];
       cvSet2D(Lab,i,j,value);
     }
   }
   cvReleaseImage(&reference);
   cvCvtColor(Lab,bgr,CV_Lab2BGR);
   cvReleaseImage(&Lab);
   cvNamedWindow("Original Image with Natural Vignetting");
   cvShowImage("Original Image with Natural Vignetting",bgr);
   cvWaitKey(0);
   cvDestroyAllWindows();
}


void main(){
   naturalVignetting();
   mechanicalVignetting(20);
   artificial_vignetting((char*)("c://P1010033.JPG"));
} 
