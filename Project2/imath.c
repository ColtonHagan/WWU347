/*
Name: Colton Hagan
Date: 3/4/21
Class: CS347
Programs: Takes in p6 image filename and creates a new p6 with laplacian filter applied,
	  prints runtime of applying the filter, and reduces runtime using threads
*/
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

/* This is the thread function, it takes in paramater, applies the filter to a given region
   of the image using convolution, and updates the result image with the values found from the filter*/
void *threadfn(void* param) {
	struct parameter *params = (struct parameter*) param;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	int laplacian[filterWidth][filterHeight] =
	{
	   -1, -1, -1,
	   -1,  8, -1,
	   -1, -1, -1,
	};
	int image_width = params->w;
	int image_height = params->h;
	int x_coordinate, y_coordinate, red, green, blue;
	//Loops through original image 
	for(int w = 0; w < image_width; w++) {
		for (int h = params->start; h < params->start + params->size; h++) {
			red = blue = green = 0;
			//Loops through filter
			for(int i_filter_width = 0; i_filter_width < filterWidth; i_filter_width++) {
				for(int i_filter_height = 0; i_filter_height < filterHeight; i_filter_height++) {
					//Calculates rgb data for a given pixel
					x_coordinate = (w - filterWidth / 2 + i_filter_width + image_width) % image_width;
                                	y_coordinate = (h - filterHeight / 2 + i_filter_height + image_height) % image_height;
					red += params->image[y_coordinate * image_width + x_coordinate].r * laplacian[i_filter_height][i_filter_width];
					green += params->image[y_coordinate * image_width + x_coordinate].g * laplacian[i_filter_height][i_filter_width];
					blue += params->image[y_coordinate * image_width + x_coordinate].b * laplacian[i_filter_height][i_filter_width];
				}
			}
			//Checks to see if RGB values are in range
			red = inRange(red);
			blue = inRange(blue);
			green = inRange(green);
			//Locks threads and assigns new value
			pthread_mutex_lock(&mutex);
			params->result[h * image_width + w].r = red;
			params->result[h * image_width + w].g = green;
			params->result[h * image_width + w].b = blue;
			pthread_mutex_unlock(&mutex);
		}
	}
	return NULL;
}

/* Creates a new P6 image file writing both the header (using width and height) and pixel data*/
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

/* Reads in a P6 file with the given filename header and pixel data,
   checking to make sure the format and data instead the p6 file is correct*/
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
	fscanf(fp, " %ld %ld\n", width, height);
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

/* Takes in the given image, and creates threads dividing up the work of applying the filters to the image,
   it also finds the time it takes to do this*/
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


/*The driver of the program. Check for the correct number of argument.
  Read the image that is passed as an argument. Apply the filter. 
  Print elapsed time. Save the result image in a file called laplacian.ppm.*/
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
	free(img);
	free(final_image);
	return 0;
}
