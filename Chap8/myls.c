/************* myls.c file **********/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <libgen.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>

struct stat mystat, *sp;
char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";
int ls_file(char *fname)
{
	struct stat fstat, *sp;
	int r, i;
	char ftime[640];
	char linkname[100];
	sp = &fstat;

	if ( (r = lstat(fname, &fstat)) < 0){
		perror(fname); 
		exit(1);
	}
	if ((sp->st_mode & 0xF000) == 0x8000) // if (S_ISREG())
		printf("%c",'-');
	if ((sp->st_mode & 0xF000) == 0x4000) // if (S_ISDIR())
		printf("%c",'d');
	if ((sp->st_mode & 0xF000) == 0xA000) // if (S_ISLNK())
		printf("%c",'l');
	for (i=8; i >= 0; i--){
	if (sp->st_mode & (1 << i)) // print r|w|x
		printf("%c", t1[i]);
	else
		printf("%c", t2[i]);// or print -
	}
	printf("%4d ",sp->st_nlink); // link count
	
	printf("%4d ",sp->st_uid);   // uid
	
	printf("%4d ",sp->st_gid);   // gid
	
	printf("%8ld ",sp->st_size);  // file size
	// print time
	strcpy(ftime, ctime(&sp->st_mtime)); // print time in calendar form
	ftime[strlen(ftime)-1] = 0;// kill \n at end
	printf("%s ",ftime);
	
	// print name
	printf("%s", basename(fname)); // print file basename
	// print -> linkname if symbolic file
	if ((sp->st_mode & 0xF000)== 0xA000){
		// use readlink() to read linkname
		int size = readlink(fname, linkname, sizeof(linkname));
		linkname[size]='\0';
		printf(" -> %s", linkname); // print linked name
		
	}
	printf("\n");
}
int ls_dir(char *dirname){
	DIR 		*dir_p;     // directory pointer
	struct dirent   *dir_ent_p; // directory entry pointer
	chdir(dirname); // changing the directory
	if((dir_p = opendir(dirname)) == NULL){
	  	printf("can't open directory %s\n", dirname);
	    	exit(1);
	}else{
		while((dir_ent_p = readdir(dir_p)) != NULL){
			ls_file(dir_ent_p->d_name);
		}
	}
	closedir(dir_p);
}

int main(int argc, char *argv[])
{
	struct stat mystat, *sp = &mystat;
	int r;
	char *filename, path[1024], cwd[256];
	filename = "./";
	
	// default to CWD
	if (argc > 1){ 
		filename = argv[1]; // if specified a filename
	}
	if (r = lstat(filename, sp) < 0){
		printf("no such file %s\n", filename);
		exit(1);
	}
	strcpy(path, filename);
	if (path[0] !='/' && path[0] !='~'){ // path[0] != "/"  filename is relative : get CWD path
		getcwd(cwd, 256);
		strcpy(path, cwd); strcat(path, "/"); strcat(path,filename);	
	}
	if (S_ISDIR(sp->st_mode)){
		ls_dir(path);
	}
	else{
		ls_file(path);
	}
}
