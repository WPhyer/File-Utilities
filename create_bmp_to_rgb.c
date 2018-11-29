/**** Compile this code: cc create_bmp_to_rgb.c -o create_bmp_to_rgb ****/
/**** run it in the same directory as the gimp saved c source images ****/

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    /* args: image name
     *
     * steps:
     * open <image name>.c file
     * create gimp_bmp_to_rgb.c code
     * compile new code
     * execute code
     * done
     *
     * this actually hurt a little to write since some good coding rules are being broken
     * 
     */
    
    char *buffer;
    char runbuffer[16384];
    char image_name_c[80];
    long file_size;
    size_t value;
    
    snprintf(image_name_c, 80, "%s.c", argv[1]);
    fprintf(stdout, "Creating code for %s\r\n", image_name_c);
    
    FILE *source_code = fopen("gimp_bmp_to_rgb.c", "w");
    FILE *gimp_c = fopen(image_name_c, "r");
    FILE *command;
    
    if(gimp_c == NULL)
    {
        fprintf(stderr, "File error opening %s\r\n", image_name_c);
        exit(1);
    }
    
    if(source_code == NULL)
    {
        fputs("File error creating 'gimp_bmp_to_rgb.c'\r\n", stderr);
        exit(2);
    }
    

    /***** create buffer to store the image file *****/
    fseek(gimp_c, 0L, SEEK_END);
    file_size = ftell(gimp_c);
    rewind(gimp_c);
    buffer = (char *)malloc(sizeof(char) * file_size);
    if(buffer == NULL)
    {
        fprintf(stderr, "Memory error trying to allocate %ld bytes\r\n", file_size);
        exit(3);
    }
    value = fread(buffer, 1, file_size, gimp_c);
    if(value != file_size)
    {
        fprintf(stderr, "Error reading %s\r\n", image_name_c);
        exit(4);
    }
    fclose(gimp_c);
    

    /***** create the source code file *****/
    fprintf(source_code, "#include <stddef.h>\r\n");
    fprintf(source_code, "#include <stdio.h>\r\n");
    fprintf(source_code, "#include <stdint.h>\r\n");
    fprintf(source_code, "\r\n");
    fwrite(buffer, sizeof(char), file_size, source_code);
    fprintf(source_code, "\r\n");
    
    fprintf(source_code, "int main(int argc, char *argv[])\r\n");
    fprintf(source_code, "{\r\n");
    fprintf(source_code, "	int row = 0;\r\n");
    fprintf(source_code, "	int column = 0;\r\n");
    fprintf(source_code, "	int16_t offset;\r\n");
    fprintf(source_code, "	int16_t length;\r\n");
    fprintf(source_code, "	int16_t rgbData;\r\n");
    fprintf(source_code, "	int16_t height = (int16_t)%s_image.height;\r\n", argv[1]);
    fprintf(source_code, "	int16_t width = (int16_t)%s_image.width;\r\n", argv[1]);
    fprintf(source_code, "\r\n");
    fprintf(source_code, "	FILE *foo = fopen(\"rgb/%s.rgb\", \"w+\");\r\n", argv[1]);
    fprintf(source_code, "\r\n");
    fprintf(source_code, "	if(foo == ((void *)0))\r\n");
    fprintf(source_code, "	{\r\n");
    fprintf(source_code, "		printf(\"Cannot open 'rgb/%s.rgb'\\n\");\r\n", argv[1]);
    fprintf(source_code, "		return -1;\r\n");
    fprintf(source_code, "	}\r\n");
    fprintf(source_code, "\r\n");
    fprintf(source_code, "	// height | width\r\n");
    fprintf(source_code, "	fwrite(&width, sizeof(int16_t), 1, foo);\r\n");
    fprintf(source_code, "	fwrite(&height, sizeof(int16_t), 1, foo);\r\n");
    fprintf(source_code, "\r\n");
    fprintf(source_code, "	for(row = 0; row < %s_image.height; row++)\r\n", argv[1]);
    fprintf(source_code, "	{\r\n");
    fprintf(source_code, "		// offset | length | data\r\n");
    fprintf(source_code, "		offset = 0;\r\n");
    fprintf(source_code, "		length = (int16_t)%s_image.width;\r\n", argv[1]);
    fprintf(source_code, "		fwrite(&offset, sizeof(int16_t), 1, foo);\r\n");
    fprintf(source_code, "		fwrite(&length, sizeof(int16_t), 1, foo);\r\n");
    fprintf(source_code, "\r\n");
    fprintf(source_code, "		for(column = 0; column < %s_image.width * %s_image.bytes_per_pixel; column += %s_image.bytes_per_pixel)\r\n", argv[1], argv[1], argv[1]);
    fprintf(source_code, "		{\r\n");
    fprintf(source_code, "			/* reverse byte order */\r\n");
    fprintf(source_code, "			rgbData = %s_image.pixel_data[(row * (%s_image.width * %s_image.bytes_per_pixel)) + column + 1] << 8;\r\n", argv[1], argv[1], argv[1]);
    fprintf(source_code, "			rgbData |= %s_image.pixel_data[(row * (%s_image.width * %s_image.bytes_per_pixel)) + column];\r\n", argv[1], argv[1], argv[1]);
    fprintf(source_code, "			fwrite(&rgbData, sizeof(int16_t), 1, foo);\r\n");
    fprintf(source_code, "		}\r\n");
    fprintf(source_code, "	}\r\n");
    fprintf(source_code, "\r\n");
    fprintf(source_code, "	fclose(foo);\r\n");
    fprintf(source_code, "}\r\n");
    fprintf(source_code, "\r\n");
    
    fclose(source_code);
    
    
    /***** compile the code *****/
    fprintf(stdout, "Compiling the code\r\n");
    command = popen("cc gimp_bmp_to_rgb.c -o gimp_bmp_to_rgb", "r");
    if(command == NULL)
    {
        fputs("Unable to call the compiler\r\n", stderr);
        exit(5);
    }
  
    while(fgets(runbuffer, sizeof(runbuffer)-1, command) != NULL)
    {
        fprintf(stdout, "%s", runbuffer);
    }
    pclose(command);
    fprintf(stdout, "\r\n");
    
    
    /***** run the code *****/
    fprintf(stdout, "Running the code\r\n");
    command = popen("./gimp_bmp_to_rgb", "r");
    if(command == NULL)
    {
        fputs("Unable to run the code\r\n", stderr);
        exit(6);
    }
  
    while(fgets(runbuffer, sizeof(runbuffer)-1, command) != NULL)
    {
        fprintf(stdout, "%s", runbuffer);
    }
    pclose(command);
    fprintf(stdout, "\r\n");
}
