
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <string.h>
#define LS_NONE 0
#define LS_l 101
#define LS_R 102
#define LS_d 103
#define LS_F 105
#define LS_f 106
#define LS_n 107
#define LS_I 104
#define LS_A 200
#define LS_a 201
#define LS_C 300
#define LS_c 301



void show_file_info1(char* filename, struct stat* info_p)
{

    char* uid_to_name(), *ctime(), *gid_to_name(), *filemode();
    void mode_to_letters();
    char modestr[11];

    mode_to_letters(info_p->st_mode, modestr);
    printf("%s", modestr);			 //st_nlink连接该文件的硬链接数，刚建立的文件值为1  
    printf(" %4d", (int) info_p->st_nlink);
    printf(" %-8s", uid_to_name(info_p->st_uid));
    printf(" %-8s", gid_to_name(info_p->st_gid));
    printf(" %8ld", (long) info_p->st_size);
    printf(" %.12s", 4 + ctime(&info_p->st_mtime));
    printf(" %s\n", filename);
}

void show_file_info2(char* filename, struct stat* info_p)
{

    char* uid_to_name(), *ctime(), *gid_to_name(), *filemode();
    void mode_to_letters();
    char modestr[11];

    mode_to_letters(info_p->st_mode, modestr);
    printf("%s", modestr);
    printf(" %4d", (int) info_p->st_nlink);
    printf(" %-8d", info_p->st_uid);
    printf(" %-8d", info_p->st_gid);
    printf(" %8ld", (long) info_p->st_size);
    printf(" %.12s", 4 + ctime(&info_p->st_mtime));
    printf(" %s\n", filename);
}



void mode_to_letters(int mode, char str[])
{
    strcpy(str, "----------");		/* default=无参数 */  
    if (S_ISDIR(mode))
    {
        str[0] = 'd';			  /* 目录      */  
    }

    if (S_ISCHR(mode))
    {
        str[0] = 'c';			/* 字符设备   */  
    }

    if (S_ISBLK(mode))
    {
        str[0] = 'b';
    }

    if ((mode & S_IRUSR))
    {
        str[1] = 'r';			 /* 用户权限  */  
    }

    if ((mode & S_IWUSR))
    {
        str[2] = 'w';
    }

    if ((mode & S_IXUSR))
    {
        str[3] = 'x';
    }

    if ((mode & S_IRGRP))
    {
        str[4] = 'r';
    }

    if ((mode & S_IWGRP))
    {
        str[5] = 'w';
    }

    if ((mode & S_IXGRP))
    {
        str[6] = 'x';
    }

    if ((mode & S_IROTH))
    {
        str[7] = 'r';
    }

    if ((mode & S_IWOTH))
    {
        str[8] = 'w';
    }

    if ((mode & S_IXOTH))
    {
        str[9] = 'x';
    }

}


char* uid_to_name(uid_t uid)
{
    struct passwd* getpwuid(),* pw_ptr;
    static char numstr[10];
    if((pw_ptr = getpwuid(uid)) == NULL)
    {
        sprintf(numstr,"%d",uid);
        return numstr;
    }
    else
    {
        return pw_ptr->pw_name;
    }
}


char* gid_to_name(gid_t gid)
{
    struct group* getgrgid(),* grp_ptr;
    static char numstr[10]; //获取组ID  
    if(( grp_ptr = getgrgid(gid)) == NULL)
    {
        sprintf(numstr,"%d",gid);
        return numstr;
    }
    else
    {
        return grp_ptr->gr_name;
    }
}


void do_ls(char dirname[],int mode)
{
    DIR* dir_ptr;
    struct dirent* direntp;

    if ((dir_ptr = opendir(dirname)) == NULL)
    {
        fprintf(stderr, "ls2: cannot open %s \n", dirname);
    }
    else
    {
        if(mode==LS_d)
        {
            printf("%s\n", dirname);
        }
        else
        {
            char dirs[20][100];
            int dir_count = 0;
            while ((direntp = readdir(dir_ptr)) != NULL)
            {
                if(mode < 200 && direntp->d_name[0]=='.')
                {
                    continue;
                }
                char complete_d_name[200];     // 文件的完整路径
                strcpy (complete_d_name,dirname);
                strcat (complete_d_name,"/");
                strcat (complete_d_name,direntp->d_name);
                struct stat info;
                if (stat(complete_d_name, &info) == -1)
                {
                    perror(complete_d_name);
                }
                else
                {

                    if(mode == LS_l)
                    {
                        show_file_info1(direntp->d_name, &info);
                    }
                    else if(mode==LS_n)
                    {
                        show_file_info2(direntp->d_name, &info);
                    }

                    else if(mode == LS_A||mode==LS_a||mode==LS_F)
                    {
                        if(mode==LS_a)
                        {
                            printf("%s  ", direntp->d_name);
                        }
                        if(mode==LS_A)
                        {
                            if(direntp->d_name[0]!='.')
                            {
                                printf("%s  ", direntp->d_name);
                            }
                        }
                        if(mode==LS_F)
                        {
                            struct stat infomation;
                            if(direntp->d_name[0]!='.')
                            {
                                if (lstat(complete_d_name, &infomation) == -1)
                                {
                                    perror(complete_d_name);
                                }
                                if(S_ISDIR(infomation.st_mode))
                                {
                                    printf("%s/\n", direntp->d_name);
                                }
                                else if(S_ISLNK(infomation.st_mode))
                                {
                                    printf("%s@\n", direntp->d_name);
                                }
                                else if(S_ISSOCK(infomation.st_mode))
                                {
                                    printf("%s=\n", direntp->d_name);
                                }
								else if(S_ISREG(infomation.st_mode))
                                {
                                    printf("%s*\n", direntp->d_name);
                                }
                                else if(S_ISFIFO(infomation.st_mode))
                                {
                                    printf("%s|\n", direntp->d_name);
                                }
                            }
                        }
                    }

                    else if(mode == LS_NONE||mode==LS_C||mode==LS_c||mode == LS_I||mode==LS_f)
                    {
                        if(mode == LS_I)
                        {
                            printf("%llu ", direntp->d_ino);
                        }
                        printf("%s\n", direntp->d_name);
                    }

                    else if(mode == LS_R)
                    {
                        if(S_ISDIR(info.st_mode))
                        {
                            printf("%s\n", direntp->d_name);
                            strcpy (dirs[dir_count],complete_d_name);
                            dir_count++;
                        }
                        else
                        {
                            printf("%s\n", direntp->d_name);
                        }

                    }
                }
            }
            if(mode == LS_R)
            {
                int i=0;
                printf("\n");
                for(;i<dir_count;i++){
                    printf("%s:\n", dirs[i]);
                    do_ls(dirs[i],LS_R);
                    printf("\n");
                }
            }
        }
        closedir(dir_ptr);
    }
}


int analyzeParam(char* input)
{
    if(strlen(input)==2)
    {
        if(input[1]=='l') return LS_l;
        if(input[1]=='A') return LS_A;
		if(input[1]=='a') return LS_a;
        if(input[1]=='d') return LS_d;
        if(input[1]=='R') return LS_R;
        if(input[1]=='F') return LS_F;
        if(input[1]=='f') return LS_F;
        if(input[1]=='i') return LS_I;
        if(input[1]=='C') return LS_C;
        if(input[1]=='c') return LS_c;
        if(input[1]=='n') return LS_n;
    }
    return -1;
    }


int main(int ac,char* av[])
{
    if(ac == 1)
    {
        do_ls(".",LS_NONE);
    }
    else
    {
        int mode = LS_NONE; 
        int have_file_param = 0;					// 是否有输入文件参数
        while(ac>1)
        {
            ac--;
            av++;
            int calMode = analyzeParam(*av);
            if(calMode!=-1)
            {
                mode+=calMode;
            }
            else
            {
                have_file_param = 1;
                do
                {
                    printf("%s:\n", *av);
                    do_ls(*av,mode);
                    printf("\n");
                    ac--;
                    av++;
                }while(ac>=1);
            }
        }

        if (!have_file_param)
        {
            do_ls(".",mode);
        }

    }
}
