#include <stdlib.h>

// TODO: (Kapsy) A string where the length is a word at the start of the pointer?
struct string
{
    char *Chars;
    int Length;
};


inline int
StringLength(char *Chars)
{
    int Result = 0;
    while(*Chars++ != '\0')
    {
        ++Result;
    }

    return(Result);
}

inline string
CreateString(char *Chars)
{
    string Result = {};
    Result.Chars = Chars;
    Result.Length = StringLength(Chars);

    return(Result);
}

inline int
StringCopy(char *A, char *B)
{
    int Count = 0;
    while(*A)
    {
        *B++ = *A++;
        ++Count;
    }

    return(Count);
}

inline int
StringCopy(string A, char *B)
{
    int Count = 0;
    while(Count < A.Length)
    {
        *B++ = *(A.Chars + Count);
        ++Count;
    }

    return(Count);
}

#if 0
static bool
StringCmp(char *A, char *B)
{
    bool Result = true;
    for(;;)
    {
        if(*A++ != *B++)
        {
            Result = false;
            break;
        }
    }

    return(Result);
}
#endif

static bool
StringsAreEqual(string A, string B)
{
    bool Result = false;
    if(A.Length == B.Length)
    {
        Result = true;

        for(int Index = 0;
                Index < A.Length;
                ++Index)
        {
            if(A.Chars[Index] != B.Chars[Index])
            {
                Result = false;
                break;
            }
        }
    }
    return(Result);
}


// TODO: (Kapsy) This is really janky! Just want to use one or the other string type.
// Although, I have a feeling that I can't use the pointer + length version without metaprogramming.
inline bool
StringsAreEqual(string A, char *B)
{
    bool Result = true;

    for(int Index = 0;
            Index < A.Length;
            ++Index)
    {

        if(*B == 0)
        {
            Result = false;
            break;
        }

        if(A.Chars[Index] != *B++)
        {
            Result = false;
            break;
        }
    }

    if(*B != 0)
    {
        Result = false;
    }

    return(Result);
}

static bool
StringStartsWith(string A, string Start)
{
    bool Result = false;

    if(A.Length >= Start.Length)
    {
        A.Length = Start.Length;

        Result = StringsAreEqual(A, Start);
    }

    return(Result);
}

static bool
StringEndsWith(string A, string End)
{
    bool Result = false;

    if(End.Length <= A.Length)
    {
        string AEnd = { A.Chars + (A.Length - End.Length), End.Length };
        Result = StringsAreEqual(AEnd, End);
    }

    return(Result);
}

inline bool
StringContainsChar(string A, char B)
{
    bool Result = false;

    char *AAt = A.Chars;
    int ARemaining = A.Length;

    while(ARemaining--)
    {
        if(*AAt == B)
        {
            Result = true;
            break;
        }
    }

    return(Result);
}

// TODO: (Kapsy) Let user malloc!
#if 0
static char *
ConcatStrings(char *A, string B)
{
    int ResultLength = StringLength(A) + B.Length + 1;

    char *Result = (char *)malloc(ResultLength);
    char *ResultAt = Result;

    char *AAt = A;
    char *BAt = B.Chars;

    int BPos = 0;

    while(*AAt)
    {
        *ResultAt++ = *AAt++;
    }
    while(BPos++ < B.Length)
    {
        *ResultAt++ = *BAt++;
    }
    *ResultAt = '\0';
    return(Result);
}
#endif

internal string
ConcatStrings(char *Buffer, u32 *Written, string A, string B)
{
    u32 Length = A.Length + B.Length;
    string Result = { Buffer, Length };
    char *ResultAt = Result.Chars;

    u32 ARemaining = A.Length;
    char *AAt = A.Chars;
    while(ARemaining--)
    {
        *ResultAt++ = *AAt++;
    }

    u32 BRemaining = B.Length;
    char *BAt = B.Chars;
    while(BRemaining--)
    {
        *ResultAt++ = *BAt++;
    }

    *ResultAt = '\0';
    *Written = (u32)(ResultAt - Buffer);

    return(Result);
}

internal string
ConcatStrings(string A, string B)
{
    u32 Length = A.Length + B.Length;
    char *Chars = (char *)malloc(Length + 1);

    string Result = { Chars, Length };
    char *ResultAt = Result.Chars;

    u32 ARemaining = A.Length;
    char *AAt = A.Chars;
    while(ARemaining--)
    {
        *ResultAt++ = *AAt++;
    }

    u32 BRemaining = B.Length;
    char *BAt = B.Chars;
    while(BRemaining--)
    {
        *ResultAt++ = *BAt++;
    }

    *ResultAt = '\0';
    return(Result);
}
