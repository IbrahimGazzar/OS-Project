#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h> 
#include <pthread.h>
#include <sys/stat.h> 
#include <sys/sysinfo.h>
#include <unistd.h>

int total_threads; 
int page_size; 
int num_files; 
int isComplete=0;  
int total_pages; 
int q_head=0; 
int q_tail=0; 
#define q_capacity 10 
int q_size=0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER, filelock=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER, fill = PTHREAD_COND_INITIALIZER;
int* pages_per_file;

struct output {
	char* data;
	int* count;
	int size;
}*out;

struct buffer {
    char* address;
    int file_number; 
    int page_number; 
    int last_page_size; 
}buf[q_capacity];

struct fd{
	char* addr;
	int size;
}*files;

void put(struct buffer b){
  	buf[q_head] = b; 
  	q_head = (q_head + 1) % q_capacity;
  	q_size++;
}

struct buffer get(){
  	struct buffer b = buf[q_tail]; 
	q_tail = (q_tail + 1) % q_capacity;
  	q_size--;
  	return b;
}

void* producer(void *arg){
	char** filenames = (char **)arg;
	struct stat sb;
	char* map; 
	int file;
	for(int i=0;i<num_files;i++){
		file = open(filenames[i], O_RDONLY);
		int pages_in_file=0; 
		int last_page_size=0;
		
		if(file == -1){ 
			printf("Error: File didn't open\n");
			exit(1);
		}

		if(fstat(file,&sb) == -1){ 
			close(file);
			printf("Error: Couldn't retrieve file stats");
			exit(1);
		}
        	if(sb.st_size==0){
               		continue;
        	}
		pages_in_file=(sb.st_size/page_size);
		if(((double)sb.st_size/page_size)>pages_in_file){ 
			pages_in_file+=1;
			last_page_size=sb.st_size%page_size;
		}
		else{ 
			last_page_size=page_size;
		}
		total_pages+=pages_in_file;
		pages_per_file[i]=pages_in_file;
		map = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, file, 0); 														  
		if (map == MAP_FAILED) { 
			close(file);
			printf("Error mmapping the file\n");
			exit(1);
    	}		
		for(int j=0;j<pages_in_file;j++){
			pthread_mutex_lock(&lock);
			while(q_size==q_capacity){
			    pthread_cond_broadcast(&fill); 
				pthread_cond_wait(&empty,&lock); 
			}
			pthread_mutex_unlock(&lock);
			struct buffer temp;
			if(j==pages_in_file-1){ 
				temp.last_page_size=last_page_size;
			}
			else{
				temp.last_page_size=page_size;
			}
			temp.address=map;
			temp.page_number=j;
			temp.file_number=i;
			map+=page_size; 
			pthread_mutex_lock(&lock);
			put(temp);
			pthread_mutex_unlock(&lock);
			pthread_cond_signal(&fill);
		}
		close(file);
	}
	isComplete=1;
	
	pthread_cond_broadcast(&fill);
	return 0;
}

struct output RLECompress(struct buffer temp){
	struct output compressed;
	compressed.count=malloc(temp.last_page_size*sizeof(int));
	char* tempString=malloc(temp.last_page_size);
	int countIndex=0;
	for(int i=0;i<temp.last_page_size;i++){
		tempString[countIndex]=temp.address[i];
		compressed.count[countIndex]=1;
		while(i+1<temp.last_page_size && temp.address[i]==temp.address[i+1]){
			compressed.count[countIndex]++;
			i++;
		}
		countIndex++;
	}
	compressed.size=countIndex;
	compressed.data=realloc(tempString,countIndex);
	return compressed;
}

int calculateOutputPosition(struct buffer temp){
	int position=0;
	
	for(int i=0;i<temp.file_number;i++){
		position+=pages_per_file[i];
	}
	position+=temp.page_number;
	return position;
}

void *consumer(){
	do{
		pthread_mutex_lock(&lock);
		while(q_size==0 && isComplete==0){
		    pthread_cond_signal(&empty);
			pthread_cond_wait(&fill,&lock); 
		}
		if(isComplete==1 && q_size==0){ 
			pthread_mutex_unlock(&lock);
			return NULL;
		}
		struct buffer temp=get();
		if(isComplete==0){
		    pthread_cond_signal(&empty);
		}	
		pthread_mutex_unlock(&lock);
		
		int position=calculateOutputPosition(temp);
		out[position]=RLECompress(temp);
	}while(!(isComplete==1 && q_size==0));
	return NULL;
}

void printOutput(){
	char* finalOutput=malloc(total_pages*page_size*(sizeof(int)+sizeof(char)));
   
	for(int i=0;i<total_pages;i++){
		if(i<total_pages-1){
			if(out[i].data[out[i].size-1]==out[i+1].data[0]){ 
				out[i+1].count[0]+=out[i].count[out[i].size-1];
				out[i].size--;
			}
		}
		
		for(int j=0;j<out[i].size;j++){
			int num=out[i].count[j];
			char character=out[i].data[j];
			*((int*)finalOutput)=num;
			finalOutput+=sizeof(int);
			*((char*)finalOutput)=character;
            finalOutput+=sizeof(char);
                       if (character=='\n'){
                       printf("\n");
                       continue;
                       }
                       else{
			printf("%d%c",num,character);
			}
		}
	}

}

void freeMemory(){
	
	free(pages_per_file);
	for(int i=0;i<total_pages;i++){
		free(out[i].data);
		free(out[i].count);
	}
	free(out);

}

int main(int argc, char* argv[]){
	
	if(argc<2){
		printf("pzip: file1 [file2 ...]\n");
		exit(1);
	}

	page_size = 10000000;
	num_files=argc-1; 
	total_threads=get_nprocs(); 
	pages_per_file=malloc(sizeof(int)*num_files); 
	
    out=malloc(sizeof(struct output)* 512000*2); 
	
	pthread_t pid,cid[total_threads];
	pthread_create(&pid, NULL, producer, argv+1); 

	for (int i = 0; i < total_threads; i++) {
        pthread_create(&cid[i], NULL, consumer, NULL);
    }

    for (int i = 0; i < total_threads; i++) {
        pthread_join(cid[i], NULL);
    }
    pthread_join(pid,NULL);
	printOutput();
	return 0;
}
