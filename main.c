#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
    int ok=1;

    char* output=NULL;
    int direct=1;
    char* input="\0";

    for(int i=1;i<argc;i++)
    {
        char* arg=argv[i];
        if(strcmp(arg, "-o")==0)
        {
            i++;
            output=argv[i];
            direct=0;
        }
        else
        {
            if(*input==0)
            {
                input=arg;
                ok=0;
            }
            else
            {
                ok=2;
            }
        }
    }

    if(ok==1)
    {
        printf("\x1b[1mstrip-gcode: \x1b[31mfatal-error:\x1b[0m no input file\n");
        return 2;
    }
    if(ok==2)
    {
        printf("\x1b[1mstrip-gcode: \x1b[31merror:\x1b[0m too many arguments\n");
        return 2;
    }

    FILE* in=fopen(input, "r");
    if(in==NULL)
    {
        printf("\x1b[1mstrip-gcode: \x1b[31mfatal-error:\x1b[0m can not read input file\n");
        return 2;
    }

    FILE* out;
    if(direct==0)
    {
        out=fopen(output, "w");
        if(out==NULL)
        {
            printf("\x1b[1mstrip-gcode: \x1b[31mfatal-error:\x1b[0m can not create input file\n");
            return 2;
        }
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    while((read=getline(&line, &len, in))!=-1)
    {
        if(line[0]!=';')
        {
            strncpy(line, line, read-1);
            line[read-1]='\0';

            char* ptr=strchr(line, '(');
            if(ptr)
            {
                int index=ptr-line;
                if(index>0)
                {
                    strncpy(line, line, index);
                    line[index]='\0';
                }
                else
                {
                    *line=0;
                }
            }
            ptr=strchr(line, ';');
            if(ptr)
            {
                int index=ptr-line;
                if(index>0)
                {
                    strncpy(line, line, index);
                    line[index]='\0';
                }
                else
                {
                    *line=0;
                }
            }


            for(size_t i=read-1;i>0 && (line[i]=='\0' || line[i]==' ');i--)
            {
                strncpy(line, line, i);
                line[i]='\0';
            }

            if(line[0]!='\r')
            {
				if(direct)
				{
					printf(line);
					if(*line)
						printf("\n");
				}
				else
				{
					fprintf(out, line);
					if(*line)
						fprintf(out, "\n");
				}
            }
        }
    }

    fclose(in);
    if(direct==0)
        fclose(out);

    return 0;
}
