#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <stdio.h>
#include <stdlib.h>

#include "util/String.cpp"
#include "Tier0.cpp"
#define INPUT_MAX 100

typedef int (Tier0::*ActionCallback)(char * input, int length);
typedef std::map<std::string, ActionCallback> ActionMap;

ActionCallback redirect(char * verb, Tier0 * t0)
{
    ActionMap actionMap;

    actionMap[(std::string) "create"] 	= &Tier0::actionCreate;
    actionMap[(std::string) "header"] 	= &Tier0::actionHeader;
    actionMap[(std::string) "insert"] 	= &Tier0::actionInsert;
    actionMap[(std::string) "display"] 	= &Tier0::actionDisplay;
    actionMap[(std::string) "delete"] 	= &Tier0::actionDelete;
    actionMap[(std::string) "search"] 	= &Tier0::actionSearch;
    actionMap[(std::string) "quit"] 	= &Tier0::actionQuit;

    ActionMap::iterator iterator = actionMap.find(verb);

    if (iterator == actionMap.end())
        return 0;
    
    return iterator->second;
};

int main(int argc, char ** argv)
{
    char * input = (char *) malloc(sizeof(char) * INPUT_MAX);


    for (int i = 1; i < argc; i ++)
    {
        sprintf(input, i + 1 == argc ? "%s%s" : "%s%s ", input, argv[i]);
    }

    Tier0 * t0 = new Tier0();

    int len;
    len = strlen(input);

    char * verb = UtilString::getFirstWord(input, len);
    ActionCallback callback = redirect(verb, t0);

    if (!callback) {
        std::cout << "Usage: ./Toy [subcommand]" << std::endl;
        return 0;
    }

    try
    {
        (*t0.*callback)(input, len);
    }
    catch(const char * errMessage)
    {
        std::cerr << "E: " << errMessage << std::endl;
    }

    return 0;
}
/*
int main()
{
    char * input = (char *) malloc(sizeof(char) * INPUT_MAX);
    Tier0 * t0 = new Tier0();

    if (!input)
    {
        std::cerr << "malloc fail initializing REPL!" << std::endl;
    }

    int len;

    while (1)
    {
        std::cout << "Toy ";

        std::cin.getline(input, INPUT_MAX);
        len = strlen(input);

        char * verb = UtilString::getFirstWord(input, len);
        ActionCallback callback = redirect(verb, t0);

        if (!callback) {
            std::cout << "E: Unknown command" << std::endl;
            continue;
        }

        int resultCode;

        try
        {
            resultCode = (*t0.*callback)(input, len);
        }
        catch(const char * errMessage)
        {
            std::cerr << "E: " << errMessage << std::endl;
            continue;
        }
        catch(int escape)
        {
            std::cerr << "E: " << "Fatal error" << std::endl;
            std::cerr << "Unrecoverable, exiting now." << std::endl;
            continue;
        }


        if (resultCode) {
            switch(resultCode)
            {
                case ResultCode::Unimplemented:
                    std::cout << "This action is not implemented at the moment." << std::endl;
                    return 0;
                    break;

                case ResultCode::GraceExit:
                    std::cout << "Goodbye" << std::endl;
                    return 0;
                    break;

                default:
                    std::cerr << "Unknown result code! Bad exit!" << std::endl;
                    return 1;
                    break;

            }

        }
    }

    return 0;
}*/
