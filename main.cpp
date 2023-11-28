#include <windows.h>
#include <stdio.h>
#include <string.h>

void sendData(const char *portname, const char *message)
{
    HANDLE hSerial;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};

    hSerial = CreateFile(portname, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (hSerial == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Napaka pri odpiranju COM porta. Preverite, če je pravilno nastavljen.\n");
        return;
    }

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        fprintf(stderr, "Napaka pri pridobivanju stanja COM porta.\n");
        CloseHandle(hSerial);
        return;
    }

    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        fprintf(stderr, "Napaka pri nastavljanju stanja COM porta.\n");
        CloseHandle(hSerial);
        return;
    }

    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts))
    {
        fprintf(stderr, "Napaka pri nastavljanju timeout-ov COM porta.\n");
        CloseHandle(hSerial);
        return;
    }

    DWORD bytes_written;
    if (!WriteFile(hSerial, message, strlen(message), &bytes_written, NULL))
    {
        fprintf(stderr, "Napaka pri pisanju v COM port.\n");
        CloseHandle(hSerial);
        return;
    }

    CloseHandle(hSerial);
}

int main()
{
    sendData("COM5", "Test.");
    return 0;
}
