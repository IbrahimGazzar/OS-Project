First we did the wzip which is a Sequential-Zip.
We gave one file to each thread. This approach isn't efficient because the file with maximum size was decreasing the compression time.
As the files can be of unequal sizes, work was not equally divided between threads.
Parallel-Zip is a better approach than Sequential-Zip because it use threads to make a parallel version of wzip so it takes less time.
There is a comparison between wzip and pzip using time instruction in linux: 

>>$ time ./test-wzip.sh
real 0m0.250s
user 0m0.163s
sys  0m0.073s

>>$ time ./test-pzip.sh
real 0m0.143s
user 0m0.035s
sys  0m0.100s



We used producer-consumer based approach. It's a single producer and multiple consumers. 
This is because a producer is only required to create memory mapping for files whereas the consumers are required to compress the files.
The file is broken into chunks by the producer and these pages pages are zipped by the consumers.

#Producer:
    -Adds to the buffer
    -Each buffer object has:
        * File number
        * Page number
        * Memory mapping

#Consumers:
    -take from the buffer
    -Each zipped object has
        * Compressed data
        * Datasize


>Flow of the project:
/Producer function to memory map the files passed as arguments.
    //Step 1: Get the file.
Source code>>   char** filenames = (char **)arg;
   
    //Step 2: Open the file
Source code>>   for(int i=0;i<num_files;i++)
file = open(filenames[i], O_RDONLY);      O_RDONLY: This flag is open for reading only.

    //Step 3: Get the file info. int fstat(int descriptor, struct stat *buffer) 
The fstat() function gets status information about the object specified by the open descriptor descriptor.
It stores the information in the area of memory indicated by the buffer argument.
Source code>>   if(fstat(file,&sb) == -1)
   
    //Step 4: Calculate the number of pages and last page size. st_size contains the size offset. 
Source code>>   pages_in_file=(sb.st_size/page_size);
    
    //Step 5: Map the entire file.
The mmap() function is used for mapping between a process address space and either files or devices.
When a file is mapped to a process address space, the file can be accessed like an array in the program.
mmap helps to reduce overhead and equally divide the workload between thread.
Source code to map the entire file>>  map = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, file, 0)
    //Step 6: For all the pages in file, create a Buffer type data with the relevant information for the consumer.
Source code>>   put(temp);

    //Step 7: Close the file.
Source code>>   close(file);

/consumer function to memory map the files passed as arguments.
We used Run-length encoding (RLE) to Compresses the buffer object.
(RLE) is a simple form of data compression, where runs (consecutive data elements) are replaced by just one data value and count.

void *consumer()   //Called by consumer threads to start memory compression
Source code>>   pthread_mutex_lock(&lock);
                    while(q_size==0 && isComplete==0)
                        pthread_cond_wait(&fill,&lock); //call the producer to start filling the queue.
                    if(isComplete==1 && q_size==0){ //If producer is done mapping and there's nothing left in the queue.
			            pthread_mutex_unlock(&lock);
			            return NULL;       
/function printOutput():
Source code>>   char* finalOutput=malloc(total_pages*page_size*(sizeof(int)+sizeof(char)));
char* init=finalOutput; //contains the starting point of finalOutput pointer
fwrite(init,finalOutput-init,1,stdout);
