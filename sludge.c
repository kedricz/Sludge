/***************************************************************************** 
 *  sludge - tape archive utility
 *  
 *  It creates archives to store and transmit multiple files as a single file. 
 *  
 *  Functions
 *  -create
 *  -list
 *  -add
 *  -remove
 *  -extract
 *
 ******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


typedef struct {
  char name[255];  // name of the file
  int size;        // size of the file
  char path[255];  // relative path
}header;



/* Creates an archive */
//
//  Ex) ./sludge -c archive_name.sludge file.1 file.2 file.3
//
void create(char* sludge, char* file) {
  FILE* fout = fopen(sludge, "w+");
  FILE* fin = fopen(file, "r");

  /* if the file does not exist */
  if(fin==NULL) {
    printf("Error opening file!\n");
    exit(1);
  }

  /* get size of the file that will be added */
  struct stat st;
  stat(file, &st);
  int size = st.st_size;

  /////////////////////////////////////////////////////////////////
  int dir;
  if(st.st_mode & S_IFDIR)
    dir = 0;
  else if(st.st_mode & S_IFREG)
    dir = -1;
  //////////////////////////////////////////////////////////////

  /* header file */
  header hd1;
  strcpy(hd1.path, file);

  
  // check if the file is in directory
  char *s;
  s = strchr(file, '/');

  // if the file is in directory
  if(s != NULL){
      char* s1;
      char* s2;
     
      s1 = strchr(s+1, '/');
      
      /* if no more subdirectory exist */
        if(s1 == NULL) {
	strcpy(hd1.name, s+1);
      }

      /* if there are more subdirectories */
        else {
	int c = -1;
	while(s1 != NULL) {
	  s2 = strchr(s1+1, '/');

	  if(s2 == NULL) 
	    break;
	
	  c = 0;
	  strcpy(s1, s2+1);
	}
	
	if(!c)
	  strcpy(hd1.name, s1);
	else
	  strcpy(hd1.name, s1+1);

      } // end else

      //printf("hd1.name = %s\n", hd1.name);
  } // end if

  
  /* if the file is not in a directory */
  else {     
    strcpy(hd1.name, file);
  }

  hd1.size = size;


  /* write header to archive file */
  fwrite(&hd1, sizeof(header), 1, fout);

  /* read and write file contents */
  char *buffer = malloc(size+1);
  fgets(buffer, size+1, fin);
  fputs(buffer, fout);

  fclose(fout);
  fclose(fin);  
}



/* Add files to the archive */
//
//  Ex) ./sludge -a archive_name.sludge file.4 file.5
//
void add(char* sludge, char* file) {
  FILE* fout = fopen(sludge, "a");
  FILE* fin = fopen(file, "r");

  /* if the file does not exist */
  if(fin==NULL) {
    printf("Error opening file!\n");
    exit(1);
  }
 
  /* get size of the file that will be added */
  struct stat st;
  stat(file, &st);
  int size = st.st_size;

  //////////////////////////////////////////////////////////////
  int dir;
  if(st.st_mode & S_IFDIR)
    dir = 0;
  else if(st.st_mode & S_IFREG)
    dir = -1;
  ///////////////////////////////////////////////////////////////

  /* header file */
  header hd2;
  strcpy(hd2.path, file);
  

  /* check if the file is in directory */                                                      
  char *s;
  s = strchr(file, '/');

  /* if the file is in directory */
  if(s != NULL) {
    char* s1;
    char* s2;
    s1 = strchr(s+1, '/');

    /* if no more subdirectory exist */
    if(s1==NULL) {
      strcpy(hd2.name, s+1);
    } 

    else {
      int c = -1;
      while(s1 != NULL) {
	s2 = strchr(s1+1, '/');
	
	if(s2==NULL)
	  break;
	c = 0;
	strcpy(s1, s2+1);
      }
      
      if(!c)
	strcpy(hd2.name, s1);
      else
	strcpy(hd2.name, s1+1);
    }
    //printf("hd2.name = %s\n", hd2.name);
  }
  
  else {
    strcpy(hd2.name, file);
  }
  hd2.size = size;



  /* write header to archive file */
  fwrite(&hd2, sizeof(header), 1, fout);

  /* read and write file contents */
  char *buffer = malloc(size+1);
  fgets(buffer,size+1, fin);
  fputs(buffer, fout);
  
  fclose(fout);
  fclose(fin);
}



/* List the contents of the archive */
//
//  Ex) ./sludge -l archive_name.sludge
//
void list(char* file) {
  FILE *fp = fopen(file, "r");
  header h;

  /* if the file does not exist */
  if(fp==NULL) {
    printf("Error opening file!\n");
    exit(1);
  }
  
  /* read the headers of the files in the archive */
  while(!feof(fp)) {
    fread(&h, sizeof(header), 1, fp);

    if(h.size == 0)
      break;
    printf("%s\n", h.path);
    //printf("%s\n", h.name);

    fseek(fp, h.size, SEEK_CUR);
    memset(&h, 0, sizeof(h));
  }

  fclose(fp);
}




/* Remove file(s) from archive file */
//
//  Ex) ./sludge -r archive_name.sludge file.3
//
void rmv(char *sludge, char* file) {
  header h;
  char *buff;
  FILE *fp;
  int check;

  if(strcmp(sludge, "temp")==0) {
    rename(sludge, "temp2");
    fp = fopen("temp2", "r");
    check = 0;
  }
  else {
    rename(sludge, "temp");
    fp = fopen("temp", "r");
    check = 1;
  }

  /* if the file does not exist */
  if(fp==NULL) {
    printf("Error opening file!\n");
    exit(1);
  }

  FILE *fout = fopen(sludge, "w");


  /* read the header and finds the file that needs to be removed */
  while(!feof(fp)) {
    fread(&h, sizeof(header), 1, fp);
                                                                       
    if(h.size == 0)
      break;

    /* if file is found, remove the file */
    if(strcmp(h.path, file)==0) {
      fseek(fp, h.size, SEEK_CUR);
    }

    else {
      fwrite(&h, sizeof(header), 1, fout);
      buff = malloc(h.size+1);
      fgets(buff,h.size+1,fp);
      fputs(buff,fout);
    }
    memset(&h, 0, sizeof(h));
    //memset(buff, 0, sizeof(buff));
    //free(buff);
  }

  fclose(fout);

  if(check)
    remove("temp");
  else
    remove("temp2");

  fclose(fp);
}



/* extract the file in the archive file */
//
// Ex) ./sludge -e archive_sludge file.2 file.5
//
void extract(char *sludge, char* file) {
  FILE *fp = fopen(sludge, "r");
  header h;
  char* fpath;
  char* buff;

  /* if the file does not exist */
  if(fp==NULL) {
    printf("Error opening file!\n");
    exit(1);
  }

  fpath = malloc(1+strlen(file));
  strcpy(fpath, file);
  

  /* read the header and finds the file that needs to extract */
  while(!feof(fp)) {
    fread(&h, sizeof(header), 1, fp);
   
    if(h.size == 0)
      break;
    
    /* if file is found, extract the file */
    if(strcmp(fpath, h.path)==0) {
  
      if(strchr(fpath, '/') != NULL) {
	 char* dir_path = malloc(strlen(file));
	 const char s[2] = "/";
	 char* token;
	 
	 token = strtok(fpath, s);
	 mkdir(token, 0777);
	 strcat(dir_path, token);

	 while(token != NULL) {
	   strcat(dir_path, "/");
	   token = strtok(NULL, s);
	   strcat(dir_path, token);
      
	   if(strcmp(token, h.name)==0)
	     break;

	   mkdir(dir_path, 0777);
	 } // end while
      } //end if '/'

     
      printf("h.path = %s\n", h.path);


      //     if(h.isDIR == 0) {
      //	mkdir(h.path, 0777);
      //}
	
      
	FILE *fout = fopen(h.path, "w");
	buff = malloc(h.size+1);
	fgets(buff,h.size+1,fp);
	fputs(buff,fout);
	fclose(fout);
	free(buff);
      
      fclose(fp);
      return;
    } // end if (file)

    fseek(fp, h.size, SEEK_CUR);
    memset(&h, 0, sizeof(h));
  }
  fclose(fp);
}



/* extract all files in the arhicve file */
//
//  Ex) ./sludge -e archive_name.sludge
// 
void extractAll(char *sludge) {
  FILE *fp = fopen(sludge, "r");
  header h;
  char *buff;
  char* fpath;

  /* if the file does not exist */
  if(fp==NULL) {
    printf("Error opening file!\n");
    exit(1);
  }

  /* read the contents of the file and extract them all */
  while(!feof(fp)) {
    fread(&h, sizeof(header), 1, fp);

    if(h.size == 0)
      break;
    
    fpath = malloc(1+strlen(h.path));
    strcpy(fpath, h.path);

    if(strchr(fpath, '/') != NULL) {
      char* dir_path = malloc(strlen(h.path));
      const char s[2] = "/";
      char* token;

      token = strtok(fpath, s);
      mkdir(token, 0777);
      strcat(dir_path, token);
      
      while(token != NULL) {
	strcat(dir_path, "/");
	token = strtok(NULL, s);
	strcat(dir_path, token);

	if(strcmp(token, h.name)==0)
	  break;
 
	mkdir(dir_path, 0777);
      }
    }


    //if(h.isDIR == 0) {
    //  mkdir(h.path, 0777);
    //}


  
      FILE *fout = fopen(h.path, "w");
      buff = malloc(h.size+1);
      fgets(buff, h.size+1, fp);
    //printf("size = %d\n", h.size);
    //printf("buff = %s\n", buff);
      fputs(buff, fout);
      free(buff);
      fclose(fout);
    

    memset(&h, 0, sizeof(h));
    memset(buff, 0, sizeof(buff));
    free(fpath);
  }

  fclose(fp);
}




int main(int argc, char *argv[]) {
 
  /* if there are not enough arguments */ 
  if(argc < 3) {
    printf("error!");
    return 0;
  }

  else {
    char* option = argv[1];

    switch(option[1]) {   
    int i=0;

    case 'c':
      create(argv[2], argv[3]);
      if(argc > 4) {
	for(i=4; i<argc; i++) 
	  add(argv[2],argv[i]);
      }
      break;
    
    case 'l':
      list(argv[2]);
      break;

    case 'a':
      for(i=3; i<argc; i++)
	add(argv[2], argv[i]);
      break;

    case 'r':
      for(i=3; i<argc; i++)
	rmv(argv[2], argv[i]);
      break;

    case 'e':
      if(argc == 3)
	extractAll(argv[2]);
      else if(argc > 3) {
	for(i=3; i<argc; i++)
	  extract(argv[2], argv[i]);
      }
      break;

    default :
      printf("Invalid option\n");
    
    } // end switch





  } // end while

  return 0;
}
