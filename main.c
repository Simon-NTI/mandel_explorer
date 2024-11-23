#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const unsigned long total_header_size = 54;
const unsigned int bits_per_pixel = 24;

const unsigned long width = 500;
const unsigned long height = 500;

const unsigned char padding = 4 - (((bits_per_pixel / 8) * width) % 4);
const unsigned long total_padding = padding * height;

const unsigned long image_size = (bits_per_pixel / 8) * (width * height) + total_padding;

const unsigned long file_size = total_header_size + image_size + total_padding;

unsigned char bitmap[file_size] = {};

void write_ulong_to_bitmap(long write_pos, unsigned long input)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            bitmap[write_pos + i] |= ((input >> (i * 8)) & (1 << j));
        }
    }
}

void plot_area(double xmin, double xmax, double ymin, double ymax)
{
    const unsigned long max_iteration = 1000;
    float greyscale = 255.0f / (float)max_iteration;

    for (unsigned long pixel_y = 0; pixel_y < height; pixel_y++)
    {
        for (unsigned long pixel_x = 0; pixel_x < width; pixel_x++)
        {
            const unsigned long pixel_index = (pixel_y * width + pixel_x);
            // const double difference = (float)pixel_index / (float)image_size;
            const unsigned long write_pos = total_header_size + pixel_index * 3;

            double x0 = xmin + (xmax - xmin) * ((float)pixel_x / (float)width);
            double y0 = ymin + (ymax - ymin) * ((float)pixel_y / (float)height);
            double x = 0;
            double y = 0;

            unsigned long iteration = 0;
            // printf("Plotting pixel %lu at (%f, %f)\n", pixel_index, x0, y0);

            while (x * x + y * y <= 2 * 2 && iteration < max_iteration)
            {
                double xtemp = x * x - y * y + x0;
                y = 2 * x * y + y0;
                x = xtemp;
                iteration++;
            }

            const unsigned char color = 255 - (unsigned char)(iteration * greyscale);

            // printf("Finished after %lu iterations with color %u\n", iteration, color);
            // printf("Finished after %lu iterations\n", iteration);

            // printf("Writing to byte %lu\n", write_pos);
            bitmap[write_pos] = color;
            bitmap[write_pos + 1] = color;
            bitmap[write_pos + 2] = color;
        }
    }
}

int main()
{
    // clang-format off
    /* identifier */
    bitmap[0] = 0x42;
    bitmap[1] = 0x4D;

    /* File size in bytes */
    write_ulong_to_bitmap(2, file_size);
    
    // bitmap[6] = 66;
    // bitmap[7] = 0;
    // bitmap[8] = 0;
    // bitmap[9] = 0;

    /* Reserved field */
    bitmap[6] = 0;
    bitmap[7] = 0;
    bitmap[8] = 0;
    bitmap[9] = 0;

    /* Offset to image data, bytes */
    // sprintf(bitmap + 10, "%ld", total_header_size);
    bitmap[10] = 54;
    bitmap[11] = 0;
    bitmap[12] = 0;
    bitmap[13] = 0;

    /* Header size in bytes */
    bitmap[14] = 40;
    bitmap[15] = 0;
    bitmap[16] = 0;
    bitmap[17] = 0;

    /* Width of image */
    write_ulong_to_bitmap(18, width);

    /* Height of image */
    write_ulong_to_bitmap(22, height);
    
    /* Number of colour planes */
    bitmap[26] = 1;
    bitmap[27] = 0;

    /* Bits per pixel */
    bitmap[28] = 24;
    bitmap[29] = 0;

    /* Compression type */
    bitmap[30] = 0;
    bitmap[31] = 0;
    bitmap[32] = 0;
    bitmap[33] = 0;

    /* Image size in bytes */
    write_ulong_to_bitmap(34, image_size);

    /* Horizontal pixels per meter */
    bitmap[38] = 0;
    bitmap[39] = 0;
    bitmap[40] = 0;
    bitmap[41] = 0;

    /* Horizontal pixels per meter */
    bitmap[42] = 0;
    bitmap[43] = 0;
    bitmap[44] = 0;
    bitmap[45] = 0;

    /* Number of colours */
    bitmap[46] = 0;
    bitmap[47] = 0;
    bitmap[48] = 0;
    bitmap[49] = 0;

    /* Important colours */
    bitmap[50] = 0;
    bitmap[51] = 0;
    bitmap[52] = 0;
    bitmap[53] = 0;

    // clang-format on

    // {
    //     printf("Painting bitmap...\n");
    //     float difference = (float)image_size / 255.0f;
    //     // printf("%f\n", difference);

    //     for (long y = 0; y < height; y++)
    //     {
    //         for (long x = 0; x < width; x++)
    //         {
    //             unsigned long pixel_index = y * width + x;
    //             unsigned long write_pos = total_header_size + pixel_index * 3 + y * padding;
    //             unsigned char color = pixel_index % 2;

    //             bitmap[write_pos] = 255 * color;
    //             bitmap[write_pos + 1] = 255 * color;
    //             bitmap[write_pos + 2] = 255 * color;
    //         }
    //     }
    // }

    const double target_x = -0.4995f;
    const double target_y = 0.52f;

    const double range = 0.0008f;

    plot_area(target_x - range / 2, target_x + range / 2, target_y - range / 2, target_y + range / 2);

    FILE *f_image = fopen("image.bmp", "wb");

    printf("--- Full Bitmap info ---\n");
    for (int i = 0; i < file_size; i++)
    {
        fputc(bitmap[i], f_image);
        // printf("Value at byte %d: %u\n", i, bitmap[i]);
    }

    fclose(f_image);
}

char *get_chars_from_long(long input)
{
    char *characters = malloc(4);
    for (int i = 0; i < 4; i++)
    {
        characters[i] = input;
        input >>= 8;
    }
    return characters;
}