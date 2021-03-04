#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>

#define THREADS 5

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

/*Checks to see if a given value is between 0 and RGB_max
  return that value if it is, and if not returning the highest/lowest
  value possible*/
int inRange(int color_value) {
	if(color_value > RGB_MAX) {
		return RGB_MAX;
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
void *threadfn(void* param) { //yet this is void in code why
	struct parameter *params = (struct parameter*) param;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	int laplacian[filterWidth][filterHeight] =
	{
	   -1, -1, -1,
	   -1,  8, -1,
	   -1, -1, -1,
	};
	int imageWidth = params->w;
	int imageHeight = params->h;
	int x_coordinate, y_coordinate, red, green, blue;
	//Loops through original image 
	for(int w = 0; w < imageWidth; w++) {
		for (int h = params->start; h < params->start + params->size; h++) {
			red = blue = green = 0;
			//Loops through filter
			for(int iteratorFilterWidth = 0; iteratorFilterWidth < filterWidth; iteratorFilterWidth++) {
				for(int iteratorFilterHeight = 0; iteratorFilterHeight < filterHeight; iteratorFilterHeight++) {
					//Calculates rgb data for a given pixel
					x_coordinate = (w - filterWidth / 2 + iteratorFilterWidth + imageWidth) % imageWidth;
                                	y_coordinate = (h - filterHeight / 2 + iteratorFilterHeight + imageHeight) % imageHeight;
					red += params->image[y_coordinate * imageWidth + x_coordinate].r * laplacian[iteratorFilterHeight][iteratorFilterWidth];
					green += params->image[y_coordinate * imageWidth + x_coordinate].g * laplacian[iteratorFilterHeight][iteratorFilterWidth];
					blue += params->image[y_coordinate * imageWidth + x_coordinate].b * laplacian[iteratorFilterHeight][iteratorFilterWidth];
				}
			}
			//Checks to see if RGB values are in range
			red = inRange(red);
			blue = inRange(blue);
			green = inRange(green);
			//Locks threads and assigns new value
			pthread_mutex_lock(&mutex);
			params->result[h * imageWidth + w].r = red;
			params->result[h * imageWidth + w].g = green;
			params->result[h * imageWidth + w].b = blue;
			pthread_mutex_unlock(&mutex);
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
	//Opens file
	fp = fopen(name, "w");
	if(!fp) {
		printf("New file can't be opened");
		exit(1);
	}
	//Prints headers
	fprintf(fp, "P6\n");
	fprintf(fp, "%ld %ld\n", width, height);
	fprintf(fp, "%d\n", RGB_MAX);
	//Prints pixel RGB data
	for(int i = 0; i < width * height; i++) {
		fwrite(&image[i], sizeof(PPMPixel), 1, fp);
	}
	//Closes file
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
	//Opens file
	fp = fopen(filename, "r");
	if(!fp) {
		printf("File can't be opened\n");
		exit(1);
    	}
	//Reads image format
	char type[2];
	fscanf(fp, "%s\n", type);
	//Checks if the image format is P6.
	if(strcmp(type, "P6") != 0) {
		printf("Not correct file format\n");
		exit(1);
	}
	//If there are comments in the file, skips them.
	char c;
	while((c = fgetc(fp)) == '#') {
		while(c != '\n') {
			fscanf(fp, "%c", &c);
		}
	}
	ungetc(c, fp);
	//reads image size information
	fscanf(fp, "%ld %ld\n", width, height);
	//Reads rgb component and check if it is equal to RGB_MAX
	int comp;
	fscanf(fp, "%d\n", &comp);
	if(comp != RGB_MAX) {
		printf("RGB component of file not equal to RRGB Max\n");
		exit(1);
	}
	img = malloc(*height * *width * sizeof(PPMPixel));
	//reads pixel data
	for(int i = 0; i < *height * *width; i++) {
		fread(&img[i], sizeof(PPMPixel), 1, fp);
	}
	//closes file
	fclose(fp);
	return img;
}

/* Create threads and apply filter to image.
 Each thread shall do an equal share of the work, i.e. work=height/number of threads.
 Compute the elapsed time and store it in *elapsedTime (Read about gettimeofday).
 Return: result (filtered image)
 */
PPMPixel *apply_filters(PPMPixel *image, unsigned long w, unsigned long h, double *elapsedTime) {
	struct timeval start_time;
	struct timeval end_time;
	gettimeofday(&start_time, NULL);
	PPMPixel *result;
	result = malloc(w*h*sizeof(PPMPixel));
	struct parameter params[THREADS];
	//Create threads and applies filter
	pthread_t threads[THREADS];
	int work = h/THREADS;
	for(int i = 0; i < THREADS; i++) {
		struct parameter *param = &params[i];
		param = malloc(sizeof(struct parameter));
		param->image = image;
		param->result = result;
		param->w = w;
		param->h = h;
		param->start = i*work;
		//If last thread does remaining work
		if(i == THREADS - 1) {
    			param->size = h - param->start;
		} else {
			param->size = work;
		}
		pthread_create(&threads[i], NULL, threadfn, (void*) param);
	}
	//Lets threads wait till they all finish their work.
	for(int i = 0; i < THREADS; i++) {
		void* res;
		pthread_join(threads[i], &res);
	}
	//Finds how long this proccess took
	gettimeofday(&end_time, NULL);
	*elapsedTime = (double) (end_time.tv_usec - start_time.tv_usec) / 1000000 + (double) (end_time.tv_sec - start_time.tv_sec);
	return result;
}


/*The driver of the program. Check for the correct number of arguments. If wrong print the message: "Usage ./a.out filename"
    Read the image that is passed as an argument at runtime. Apply the filter. Print elapsed time in .3 precision (e.g. 0.006 s). Save the result image in a file called laplacian.ppm. Free allocated memory.
 */
int main(int argc, char *argv[]) {
	//Checks if right number of args
	if(argc != 2) {
		printf("Usage ./a.out filename\n");
		return 1;
	}
	unsigned long int w, h;
	double elapsedTime = 0.0;
	char* filename = argv[1];

	PPMPixel *img;
	PPMPixel *final_image;

	img = readImage(filename, &w, &h);
	final_image = apply_filters(img, w, h, &elapsedTime);
	printf("Elapsed time : %.3f seconds\n", elapsedTime);
	writeImage(final_image, "laplacian.ppm", w, h);
	return 0;
}
