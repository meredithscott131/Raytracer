#ifndef __PPM_IMAGEWRITER_H_
#define __PPM_IMAGEWRITER_H_

#include "ImageLoader.h"
#include <fstream>
#include <iostream>
#include <GL/gl.h>

/**
 * @brief This class is used to save an image in the ASCII PPM format (P3)
 * 
 */
class PPMImageWriter {

public:
    GLubyte* image; // Pointer to the image data
    int width;     // Width of the image
    int height;    // Height of the image

    PPMImageWriter(GLubyte* image, int width, int height) {
        this->image = image;
        this->width = width;
        this->height = height;
    }

    void save(const std::string& filename) {
        if (!image || width <= 0 || height <= 0) {
            throw std::runtime_error("Invalid image data");
        }

        std::ofstream outFile(filename.c_str());
        if (!outFile.is_open()) {
            throw std::runtime_error("Could not open file for writing!");
        }

        std::cout << "Saving image to file..." << std::endl;

        // Write the header
        outFile << "P3\n";
        outFile << "# Created by PPMImageWriter\n";
        outFile << width << " " << height << "\n";
        outFile << "255\n"; // Max color value

        // Write pixel data
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                int idx = 3 * ((height - 1 - i) * width + j);
                int r = static_cast<int>(image[idx]);
                int g = static_cast<int>(image[idx + 1]);
                int b = static_cast<int>(image[idx + 2]);
                outFile << r << " " << g << " " << b << " ";
            }
            outFile << "\n";
        }

        outFile.close();
        std::cout << "Image saved successfully!" << std::endl;
    }
};

#endif