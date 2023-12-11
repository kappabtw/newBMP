#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <stdint.h>

using namespace std;


#pragma pack(push, 1)
struct BMPHeader {
    uint16_t signature;        
    uint32_t img_size;         
    uint32_t reserved;
    uint32_t data_offset;      
    uint32_t header_size;      
    int32_t width;             
    int32_t height;            
    uint16_t color_planes;    
    uint16_t bits_per_pixel;   
    uint32_t compression;      
    uint32_t image_size;       
    int32_t x_pixels_per_meter; 
    int32_t y_pixels_per_meter; 
    uint32_t colors_used;     
    uint32_t important_colors; 
};
#pragma pack(pop)


struct pixel_struct {
    uint8_t red;   
    uint8_t green; 
    uint8_t blue;  
};

void rotate_left(vector<pixel_struct>& pix, BMPHeader& header) {
    vector<pixel_struct> new_pix(pix.size()); 
    int width = header.width; 
    int height = header.height; 

    
    for (int y = 0; y < height; y++) { 
        for (int x = 0; x < width; x++) { 
            int new_x = height - y - 1; 
            int new_y = x; 

            int original_index = y * width + x; 
            int new_index = new_y * height + new_x; 

            new_pix[new_index] = pix[original_index]; 
        }
    }
    
    header.width = height; 
    header.height = width; 

    
    pix = new_pix; 
}


void rotate_right(vector<pixel_struct>& pix, BMPHeader& header) {
    vector<pixel_struct> new_pix(pix.size()); 
    int width = header.width; 
    int height = header.height; 

    
    for (int y = 0; y < height; y++) { 
        for (int x = 0; x < width; x++) { 
            int new_x = y; 
            int new_y = width - x - 1; 

            int original_index = y * width + x; 
            int new_index = new_y * height + new_x; 

            new_pix[new_index] = pix[original_index]; 
        }
    }
    
    header.width = height; 
    header.height = width; 

    
    pix = new_pix; 
}


void gauss(vector<pixel_struct>& pix, int width, int height, int radius) {
    vector<pixel_struct> blur(width * height); 

    
    vector<double> kernel(2 * radius + 1); 
    double sigma = static_cast<double>(radius) / 3.0; 
    double sum = 0.0; 

    for (int x = -radius; x <= radius; ++x) { 
        double value = exp(-0.5 * (x * x) / (sigma * sigma)); 
        kernel[x + radius] = value; 
        sum += value; 
    }

    for (int i = 0; i < 2 * radius + 1; ++i) { 
        kernel[i] /= sum;
    }

    
    for (int y = 0; y < height; ++y) { 
        for (int x = 0; x < width; ++x) { 
            double r = 0.0, g = 0.0, b = 0.0; 
            int index = y * width + x; 

            for (int i = -radius; i <= radius; ++i) { 
                int neighborX = x + i; 
                if (neighborX >= 0 && neighborX < width) { 
                    int neighborIndex = y * width + neighborX; 
                    double weight = kernel[i + radius]; 

                    r += pix[neighborIndex].red * weight; 
                    g += pix[neighborIndex].green * weight; 
                    b += pix[neighborIndex].blue * weight; 
                }
            }

            blur[index].red = static_cast<unsigned char>(r); 
            blur[index].green = static_cast<unsigned char>(g); 
            blur[index].blue = static_cast<unsigned char>(b); 
        }
    }
    
    pix = blur; 
}

int main() {

    bool continueWork = true;
    while (continueWork)
    {
        cout << "Enter name of your image (without .bmp):";
        string file_name;
        string newfile_name;
        cin >> file_name;
        file_name = file_name + ".bmp";
        newfile_name = "new" + file_name + ".bmp";

        fstream img(file_name, ios::binary);

        bool correctInput = false;
        
        while (correctInput == false)
        {
            ifstream img(file_name, ios::binary);
            if (!img.is_open()) {
                cerr << "Error!" << endl;
                system("pause");
                return 0;
            }
            correctInput = true;
        }
        ofstream newbmp(newfile_name, ios::binary | ios::trunc);

        
        BMPHeader header;
        img.read(reinterpret_cast<char*>(&header), sizeof(header));

        int width = header.width;
        int height = header.height;
        char* garbage = new char[header.data_offset - 54];

        
        vector<pixel_struct> pix(width * height);

        img.read(reinterpret_cast<char*>(pix.data()), pix.size() * 3);
        img.read(reinterpret_cast<char*>(garbage), header.data_offset - 54);

        
        char command1;
        cout << "Enter some number " << endl;
        cout << "1 - Rotate 90 degrees to the right, 2 - Rotate 90 degrees to the left, 0 - without rotate ";
        cin >> command1;

        if (command1 == '1')
            rotate_right(pix, header); 
        else if (command1 == '2')
            rotate_left(pix, header); 

        int radius;
        cout << "Enter Gaussian blur radius" << endl;
        cout << "Enter an integer if blur required or zero if blur is not required ";
        cin >> radius;

        if (radius != 0)
            gauss(pix, width, height, radius); 

        
        newbmp.write(reinterpret_cast<char*>(&header), sizeof(header)); 
        newbmp.write(reinterpret_cast<char*>(garbage), header.data_offset - 54); 
        newbmp.write(reinterpret_cast<char*>(pix.data()), pix.size() * 3); 

        cout << "New file - 'newbmp.bmp'." << endl;

        img.close(); 
        newbmp.close(); 

        cout<<"Contuie?\n0 - no\n1 = yes"<<endl;
        int choice;
        cin>>choice;
        if (choice == 1)
        {
            continueWork = true;
        }
        else
        {
            system("pause");
            continueWork = false;
            break;
        }
    }
    return 0;
}