#include "CommandSet.h"

int CommandSet::InterpretRequest(int command, char* buffer)
{
    // zero initializes and we return zero it will be a failed attempt for the server to call back to.
    int requestComplete = 0;
    switch (command) {
    case 1:
        if (LoginUser(buffer)) {
            requestComplete = 1;
        }
        break;
    case 2:

        break;

    default:

        return requestComplete;
    }

    return requestComplete;
}

bool CommandSet::LoginUser(char* buffer)
{
    return false;
}
