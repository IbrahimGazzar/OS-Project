#include <iostream>
#include <cstdlib>

void un_zip(char *argv)
{
    char current = 0;
    int counter = 0;
    FILE *fp = fopen(argv, "rb"); //open the file in "rb" mode (opening text & non-text files for reading)
    if (fp == NULL)
    {                  //if fopen() returning a NULL
        std::cout <<"%s not found.\n"<< argv;    

        exit(1);
    }

    while (1)
    {
        //read compressed file from wzip
        int bytes = fread(&counter, 1, 4, fp); 

//** for each 4 bytes because The format of the compressed file must match exactly (a 4-byte integer followed by a character for each run) **//
        if (bytes != 4)
        {
            break;
        }
        //read 1 current item from fp stream (FILE)
        fread(&current, 1, 1, fp);
        {
            int i = 0;
            for (i = 0; i < counter; i++)
            {
                fwrite(&current, 1, 1, stdout);
                //write to [stdoutput]
            }
        }
    }
    fclose(fp);  //closes FILE stream
}


int main(int argc, char **argv) {

    if (argc < 2) //check arguments
    {
         std::cout<<"wunzip: file1 [file2 ...]\n"<<"";

        exit(1);
    }

      int i =  0;

    for (i = 1; i < argc; i++) //Cycle the contents of file and unzip each content
    {
        un_zip(argv[i]);
    }

    return 0;

}