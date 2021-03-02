#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

#include <string.h>

#define THREADS 4

#define filterWidth 3
#define filterHeight 3

#define RGB_MAX 255

typedef struct {
         unsigned char r, g, b;
} PPMPixel;

struct parameter {
        PPMPixel *image;         //original image
        PPMPixel *result;        //filtered image
        unsigned long int w;     //width of image
        unsigned long int h;     //height of image
        unsigned long int start; //starting point of work
        unsigned long int size;  //equal share of work (almost equal if odd)
};

int inRange(int color_value) {
    if(color_value > 255) {
        return 255;
    } else if (color_value < 0) {
        return 0;
    }
    return color_value;
}

/*This is the thread function. It will compute the new values for the region of image specified in params (start to start+size) using convolution.
    (1) For each pixel in the input image, the filter is conceptually placed on top ofthe image with its origin lying on that pixel.
    (2) The  values  of  each  input  image  pixel  under  the  mask  are  multiplied  by the corresponding filter values.
    (3) The results are summed together to yield a single output value that is placed in the output image at the location of the pixel being processed on the input.

 */
void *threadfn(struct parameter *params) { //yet this is void in code why

    int laplacian[filterWidth][filterHeight] = {
          -1, -1, -1,
          -1,  8, -1,
          -1, -1, -1,
    };
    int imageWidth = params->w;
    int imageHeight = params->h;
    int x_coordinate, y_coordinate;
    int red, green, blue;
    /*For all pixels in the work region of image (from start to start+size)
      Multiply every value of the filter with corresponding image pixel. Note: this is NOT matrix multiplication.
      Store the new values of r,g,b in p->result.
     */
    for(int w = 0; w < imageWidth; w++) {
        for (int h = 0; h < imageHeight; h++) {
	    red = blue = green = 0;
            for(int iteratorFilterWidth = 0; iteratorFilterWidth < filterWidth; iteratorFilterWidth++) {
                for(int iteratorFilterHeight = 0; iteratorFilterHeight < filterHeight; iteratorFilterHeight++) {
                    x_coordinate = (w - filterWidth / 2 + iteratorFilterWidth + imageWidth) % imageWidth;
                    y_coordinate = (h - filterHeight / 2 + iteratorFilterHeight + imageHeight) % imageHeight;
                    red += params->image[y_coordinate * imageWidth + x_coordinate].r * laplacian[iteratorFilterHeight][iteratorFilterWidth];
                    green += params->image[y_coordinate * imageWidth + x_coordinate].g * laplacian[iteratorFilterHeight][iteratorFilterWidth];
                    blue += params->image[y_coordinate * imageWidth + x_coordinate].b * laplacian[iteratorFilterHeight][iteratorFilterWidth];
                }
            }
            params->result[h * imageWidth + w].r = inRange(red);
            params->result[h * imageWidth + w].g = inRange(green);
            params->result[h * imageWidth + w].b = inRange(blue);
        }
    }
    return NULL;
}
/*Create a new P6 file to save the filtered image in. Write the header block
 e.g. P6
      Width Height
      Max color value
 then write the image data.
 The name of the new file shall be "name" (the second argument).
 */
void writeImage(PPMPixel *image, char *name, unsigned long int width, unsigned long int height) {
    FILE *fp;
    fp = fopen(name, "w");
    fprintf(fp, "P6\n");
    fprintf(fp, "%ld %ld\n", width, height);
    fprintf(fp, "%d\n", RGB_MAX);
    for(int i = 0; i < width*height; i++) {
        fwrite(&image[i], sizeof(PPMPixel), 1, fp);
    }
    fclose(fp);
}

/* Open the filename image for reading, and parse it.
    Example of a ppm header:    //http://netpbm.sourceforge.net/doc/ppm.html
    P6                  -- image format
    # comment           -- comment lines begin with
    ## another comment  -- any number of comment lines
    200 300             -- image width & height
    255                 -- max color value

 Check if the image format is P6. If not, print invalid format error message.
 Read the image size information and store them in width and height.
 Check the rgb component, if not 255, display error message.
 Return: pointer to PPMPixel that has the pixel data of the input image (filename)
 */
PPMPixel *readImage(const char *filename, unsigned long int *width, unsigned long int *height) {
    PPMPixel *img;
    FILE *fp;
    fp = fopen(filename, "r");
    if(!fp) {
	printf("File name not reconized");
	exit(1);
    }

    //read image format
    char type[2];
    fscanf(fp, "%s\n", type);
    //check the image format by reading the first two characters in filename and compare them to P6.
    if(strcmp(type, "P6") != 0) {
        printf("Not correct file format");
        exit(1);
    }

    //If there are comments in the file, skip them. You may assume that comments exist only in the header block.
    char c;
    fscanf(fp, "%s\n", &c);
    if(c == '#') {
        while(c != '\n') { //maybe just use s\n?
            fscanf(fp, "%c", &c);
        }
    }
    //read image size information
    fscanf(fp, "%ld %ld\n", width, height);
    //Read rgb component. Check if it is equal to RGB_MAX. If  not, display error message.
    int comp;
    fscanf(fp, "%d", &comp);
    if(comp != RGB_MAX) {
        printf("Not equal to RRGB Max");
        exit(1);
    }
    /* RGB triplet read; process it */

    //allocate memory for img. NOTE: A ppm image of w=200 and h=300 will contain 60000 triplets (i.e. for r,g,b), ---> 180000 bytes.
    img = malloc(*height * *width * 3);
    //read pixel data from filename into img. The pixel data is stored in scanline order from left to right (up to bottom) in 3-byte chunks (r g b values for each pixel) encoded as binary numbers.
    int i = 0;
    while(fread(&img[i], sizeof(PPMPixel), 1, fp)) {
        i++;
    }
    fclose(fp);
    return img;
}

/* Create threads and apply filter to image.
 Each thread shall do an equal share of the work, i.e. work=height/number of threads.
 Compute the elapsed time and store it in *elapsedTime (Read about gettimeofday).
 Return: result (filtered image)
 */
PPMPixel *apply_filters(PPMPixel *image, unsigned long w, unsigned long h, double *elapsedTime) {

    PPMPixel *result;
    //allocate memory for result
    result = malloc(w*h*3);
    //allocate memory for parameters (one for each thread)
    struct parameter *params;
    params = malloc(sizeof(struct parameter));
    params->image = image;
    params->result = result;
    params->w = w;
    params->h = h;
    params->start = 0;
    params->size = h;

    /*create threads and apply filter.
     For each thread, compute where to start its work.  Determine the size of the work. If the size is not even, the last thread shall take the rest of the work.
     */
    threadfn(params);
    //Let threads wait till they all finish their work.
    return result;
}


/*The driver of the program. Check for the correct number of arguments. If wrong print the message: "Usage ./a.out filename"
    Read the image that is passed as an argument at runtime. Apply the filter. Print elapsed time in .3 precision (e.g. 0.006 s). Save the result image in a file called laplacian.ppm. Free allocated memory.
 */
int main(int argc, char *argv[]) {
    //load the image into the buffer
    unsigned long int w, h;
    double elapsedTime = 0.0;
    char* filename = "photos/falls_2.ppm"; //Should be argv[1]
    PPMPixel *img;
    PPMPixel *final_image;
    img = readImage(filename,&w,&h);
    final_image = apply_filters(img,w,h,0);
    writeImage(final_image,"testfile.ppm",w,h); //how do we name this?
    return 0;
}
