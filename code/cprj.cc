// #include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "kapsy.h"

#define VERSION_NUMBER "1.0.0 b.1"

global_variable string VersionNumber = CreateString(VERSION_NUMBER);

enum token_type
{
    TokenType_Arg_ProjectName,
    TokenType_Arg_Blah,

    // NOTE: (Kapsy) An Element is an unrecognized block of characters.
    TokenType_Element,
    TokenType_Unknown,
};

struct token
{
    token_type Type;
    string String;
};

struct tokenizer
{
    char *At;
};

static token TokenTypeDefs[] =
{
    { TokenType_Arg_ProjectName, CreateString("-n") },
    { TokenType_Arg_Blah, CreateString("-b") },
};

enum arg_types
{
    ArgumentType_None,
    ArgumentType_ProjectName,
    ArgumentType_Blah,
    ArgumentType_Count,
};


struct file_paths
{
    string lvimrc;
    string build_sh;
    string main_cc;
};

struct files
{
    FILE *lvimrc;
    FILE *build_sh;
    FILE *main_cc;
};

global_variable string ProjectName;
global_variable string ProjectFolder;
global_variable string InstallationFolder;
global_variable string CurrentDir;
global_variable b32 GlobalBlah;

internal token
CreateTokenFromChar(char *Char)
{
    string CharAsString = CreateString(Char);
    token Result = { TokenType_Element, CharAsString };

    for(u32 TokenIndex = 0;
            TokenIndex < ArrayCount(TokenTypeDefs);
            ++TokenIndex)
    {
        token *Token = TokenTypeDefs + TokenIndex;
        if(StringsAreEqual(CharAsString, Token->String))
        {
            Result = *Token;
            break;
        }
    }
    return(Result);
}

#define DEBUG 1

int main(int argc, char **argv)
{
#if DEBUG
    printf("argc: %d argv: ", argc);
    for(u32 ArgIndex = 0;
            ArgIndex < argc;
            ++ArgIndex)
    {
        char *Arg = *(argv + ArgIndex);
        printf("%s ", Arg);
    }
    printf("\n");
#endif

    printf("cproj version %.*s by Kapsy\n",
            VersionNumber.Length, VersionNumber.Chars);

    InstallationFolder = CreateString(INSTALL_PATH);

    char CurrentDirBuffer[256];
    CurrentDir = CreateString(getcwd(CurrentDirBuffer, 256));
    if(!CurrentDir.Chars)
    {
        printf("Unable to obtain working directory!\n");
        return(EXIT_FAILURE);
    }
    CurrentDir = ConcatStrings(CurrentDir, CreateString("/"));

#if DEBUG
    printf("Installation dir: %.*s\n", InstallationFolder.Length, InstallationFolder.Chars);
    printf("Current dir: %.*s\n", CurrentDir.Length, CurrentDir.Chars);
#endif


    b32 ArgsOk = true;
    u32 ArgIndex = 1;
    for(;;)
    {
        if(ArgIndex >= argc || !ArgsOk)
        {
            break;
        }
        token Argument = CreateTokenFromChar(*(argv + ArgIndex));
        if(Argument.Type == TokenType_Arg_ProjectName)
        {
            ++ArgIndex;
            if(ArgIndex == argc)
            {
                ArgsOk = false;
            }
            else
            {
                token Value = CreateTokenFromChar(*(argv + ArgIndex));
                if((Value.Type == TokenType_Element) &&
                   (!StringContainsChar(Value.String, ' ')))
                {
                    ProjectName = Value.String;
                }
                else
                {
                    // TODO: (Kapsy) Set the error message here!
                    ArgsOk = false;
                }
            }
        }
        else if(Argument.Type == TokenType_Arg_Blah)
        {
            GlobalBlah = true;
        }
        else
        {
            // TODO: (Kapsy) Set the error message here!
            ArgsOk = false;
        }

        ++ArgIndex;
    }

    if(!ArgsOk || (ArgIndex == 1))
    {
        printf("usage: cprj [options] [[infile options] -i infile]... {[outfile options] outfile}...\n"
               "Use -h to get full help, or run \'man project_maker\'\n");
        return(EXIT_FAILURE);
    }


#if 1

    string TemplateFolder = ConcatStrings(InstallationFolder, CreateString("templates/"));
    // TODO: (Kapsy) Create a string format function!
    string ProjectFolder = ConcatStrings(ConcatStrings(CurrentDir, ProjectName), CreateString("/"));
    string CodeFolder = ConcatStrings(ProjectFolder, CreateString("code/"));
    string BuildFolder = ConcatStrings(ProjectFolder, CreateString("build/"));

    struct stat ProjectFolderStat;
    if((stat(ProjectFolder.Chars, &ProjectFolderStat) == 0) &&
       S_ISDIR(ProjectFolderStat.st_mode))
    {
        printf("Project folder already exists!\n");
        return(EXIT_FAILURE);
    }

    mkdir(ProjectFolder.Chars, ACCESSPERMS);
    mkdir(CodeFolder.Chars, ACCESSPERMS);
    mkdir(BuildFolder.Chars, ACCESSPERMS);

    file_paths TemplatePaths =
    {
        .lvimrc = ConcatStrings(TemplateFolder, CreateString(".lvimrc.template")),
        .build_sh = ConcatStrings(TemplateFolder, CreateString("build.sh.template")),
        .main_cc = ConcatStrings(TemplateFolder, CreateString("main.cc.template")),
    };

    string main_cc_FileName = ConcatStrings(ProjectName, CreateString(".cc"));
    file_paths OutputPaths =
    {
        .lvimrc = ConcatStrings(ProjectFolder, CreateString(".lvimrc")),
        .build_sh = ConcatStrings(ProjectFolder, CreateString("build.sh")),
        .main_cc = ConcatStrings(CodeFolder, main_cc_FileName),
    };

    files OutputFiles =
    {
        .lvimrc = fopen(OutputPaths.lvimrc.Chars, "w"),
        .build_sh = fopen(OutputPaths.build_sh.Chars, "w"),
        .main_cc = fopen(OutputPaths.main_cc.Chars, "w"),
    };

    int build_sh_fd = fileno(OutputFiles.build_sh);
    if(build_sh_fd >= 0)
    {
        if(fchmod(build_sh_fd, ACCESSPERMS) != 0)
        {
            printf("Could not set permissions for build.sh!\n");
        }
    }
    else
    {
        printf("Could not get file descriptor for build.sh!\n");
    }

    if((OutputFiles.lvimrc) &&
       (OutputFiles.build_sh) &&
       (OutputFiles.main_cc))
    {
        char *lvimrc_Template = ReadEntireFile(TemplatePaths.lvimrc.Chars);
        fprintf(OutputFiles.lvimrc, lvimrc_Template, ProjectName.Chars);
        fclose(OutputFiles.lvimrc);

        char *build_sh_Template = ReadEntireFile(TemplatePaths.build_sh.Chars);
        fprintf(OutputFiles.build_sh, build_sh_Template,
                ProjectName.Chars, ProjectName.Chars,
                ProjectName.Chars, ProjectName.Chars);

        // TODO: (Kapsy) If saved as h files, could do something like:
        // fprintf(OutputFiles.build_sh, build_sh_Template, TemplateArgs(build_sh)(ProjectName));
        fclose(OutputFiles.build_sh);

        char *main_cc_Template = ReadEntireFile(TemplatePaths.main_cc.Chars);
        fprintf(OutputFiles.main_cc, "%s", main_cc_Template);
        fclose(OutputFiles.main_cc);
    }
    else
    {
        printf("An error occured trying to open files for output!\n");
        return(EXIT_FAILURE);
    }

    // TODO: (Kapsy) Copy all contents of header files dir here.
    // Will just use copyfile() (BSD only) for now.

    printf("%.*s successfully created!\n", ProjectName.Length, ProjectName.Chars);

#endif
    return(EXIT_SUCCESS);
}
