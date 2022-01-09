# OS-Project
In this project, We built some UNIX utilities that are commonly used commands like wcat,wgrep,wzip,and wunzip.
Each of them had compiled successfully when compiled with the -Wall flag and passed the tests.
wcat: This tool is a simple that it reads a file provided by the user and prints its contents.
When the user wants to see the contents of file.cpp, They type: prompt> ./wcat wcat.cpp
As shown above, wcat reads the file wcat.cpp and prints out its contents.
To compile this program, we type the following: prompt> gcc -o wcat wcat.c -Wall
In the source code we typed this to print the contents: cout<<in.rdbuf();
which rdbuf returns a pointer to the stream buffer object currently associated with the stream.

wgrep: Second tool looks through the file, line by line, to find a key that provided by the user then we search in the line.
If a line contains this word within it, this line is printed out, otherwise it isn't.
In source code: getline(cin, line) , gets the line we search in.
line.find(key) != string::npos : search for the key in the line.  
As above string::npos >> It actually means until the end of the string.
This tool is case sensitive. so, if we search for done, lines with DONE will not match.
If file can't be opened, it print "wgrep: cannot open file" .

wzip and wunzip: These two tools come in a pair.(wzip) is a file compression tool, and (wunzip) is a file decompression tool.
When writing number of characters of the same type in a row, the compression tool (wzip) will turn it into the number n and a single version of that character.
The format of the compressed file is a 4-byte integer in binary format followed by the single character in ASCII.
Examble: when we write (qqqqyyy) it turns into (4q3y).
If we want to compress the file1 file1.txt into a file1.z, we type: prompt> ./wzip file.txt > file.z
The usage of The "greater than" sign is to put the output from wzip to the file file1.z instead of print it to the screen.
if no files are specified,the program will exit with return code 1 and print print "wzip: file1 [file2 ...]" as following:
if(argc==1){
  cout<<"wzip: file1 [file2 ...]"<<endl;
  exit(1);
 }

The wunzip tool does the reverse of the wzip tool, takes a compressed file and write the uncompressed results.
For example, to see the contents of file1.txt, we type: prompt> ./wunzip file1.z
The compressed file must be in form of(a 4-byte integer followed by a character for each run).
To read the compressed file from wzip: In source code: int bytes = fread(&counter, 1, 4, fp); 
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
Multiple files we pass to *wzip, they are compressed to a single compressed output, when we unzip,it will turn to a single uncompressed text.

