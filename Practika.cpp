#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <windows.h>
#include <limits>

using namespace std;

const int MAX_PLANES = 100;
const int MAX_BOARD_LEN = 40;
const int MAX_FILE_NAME = 100;
const int MAX_FIELD_LEN = 50;

struct Plane {
    int flightNumber;
    char boardNumber[MAX_BOARD_LEN];
    int passengers;
    int delayMinutes;
};

bool isNumber(const char s[]);
bool isValidBoardNumber(const char s[]);
bool parseDelay(const char s[], int& totalMinutes);
void delayToString(int totalMinutes, char result[]);
bool readPlane(ifstream& fin, Plane& plane, int lineNumber);
void selectionSortIndexes(const Plane planes[], int indexes[], int count);
void printTable(const Plane planes[], const int indexes[], int count);

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    char fileName[MAX_FILE_NAME];

    cout << "Enter data file name: ";
    cin.getline(fileName, MAX_FILE_NAME);

    if (fileName[0] == '\0') {
        cout << "Error: file name cannot be empty." << endl;
        return 1;
    }

    ifstream fin(fileName);

    if (!fin.is_open()) {
        cout << "Error: cannot open file: " << fileName << endl;
        return 1;
    }

    Plane planes[MAX_PLANES];
    int count = 0;
    int lineNumber = 0;

    while (count < MAX_PLANES && !fin.eof()) {
        lineNumber++;
        Plane tempPlane;

        if (readPlane(fin, tempPlane, lineNumber)) {
            planes[count] = tempPlane;
            count++;
        }
        else {
            fin.clear();
            fin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    fin.close();

    if (count == 0) {
        cout << "Error: file is empty or contains no valid data." << endl;
        return 1;
    }

    int indexes[MAX_PLANES];
    for (int i = 0; i < count; i++) {
        indexes[i] = i;
    }

    selectionSortIndexes(planes, indexes, count);

    cout << endl << "Sorted table by delay time:" << endl;
    printTable(planes, indexes, count);

    return 0;
}

bool isNumber(const char s[]) {
    if (s[0] == '\0') return false;
    for (int i = 0; s[i] != '\0'; i++) {
        if (!isdigit(static_cast<unsigned char>(s[i]))) {
            return false;
        }
    }
    return true;
}

bool isValidBoardNumber(const char s[]) {
    if (s[0] == '\0') return false;

    int len = strlen(s);

    if (len >= 3 &&
        (unsigned char)s[0] == 0xD0 &&
        ((unsigned char)s[1] == 0x91 || (unsigned char)s[1] == 0xB1) &&
        s[2] == '-') {

        for (int i = 3; s[i] != '\0'; i++) {
            if (!isdigit((unsigned char)s[i])) {
                return false;
            }
        }
        return (len > 3);
    }

    if (len >= 2 &&
        ((unsigned char)s[0] == 0xC1 || (unsigned char)s[0] == 0xE1) &&
        s[1] == '-') {

        for (int i = 2; s[i] != '\0'; i++) {
            if (!isdigit((unsigned char)s[i])) {
                return false;
            }
        }
        return (len > 2);
    }

    return false;
}

bool parseDelay(const char s[], int& totalMinutes) {
    if (s[0] == '\0') return false;

    char temp[MAX_FIELD_LEN];
    strcpy(temp, s);

    char* part1 = strtok(temp, ":");
    char* part2 = strtok(NULL, ":");
    char* part3 = strtok(NULL, ":");
    char* extra = strtok(NULL, ":");

    if (part1 == NULL || part2 == NULL || part3 == NULL || extra != NULL) {
        return false;
    }

    if (!isNumber(part1) || !isNumber(part2) || !isNumber(part3)) {
        return false;
    }

    int days = atoi(part1);
    int hours = atoi(part2);
    int minutes = atoi(part3);

    if (days < 0 || hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
        return false;
    }

    totalMinutes = days * 24 * 60 + hours * 60 + minutes;
    return true;
}

void delayToString(int totalMinutes, char result[]) {
    int days = totalMinutes / (24 * 60);
    totalMinutes %= 24 * 60;
    int hours = totalMinutes / 60;
    int minutes = totalMinutes % 60;
    sprintf(result, "%d:%02d:%02d", days, hours, minutes);
}

bool readPlane(ifstream& fin, Plane& plane, int lineNumber) {
    char flightStr[MAX_FIELD_LEN];
    char boardStr[MAX_BOARD_LEN];
    char passengersStr[MAX_FIELD_LEN];
    char delayStr[MAX_FIELD_LEN];

    if (!(fin >> flightStr >> boardStr >> passengersStr >> delayStr)) {
        return false;
    }

    bool hasError = false;

    if (!isNumber(flightStr)) {
        cout << "Line " << lineNumber << ": Error: invalid flight number: " << flightStr << endl;
        hasError = true;
    }

    if (!isValidBoardNumber(boardStr)) {
        cout << "Line " << lineNumber << ": Error: invalid board number: " << boardStr << endl;
        hasError = true;
    }

    if (!isNumber(passengersStr)) {
        cout << "Line " << lineNumber << ": Error: invalid number of passengers: " << passengersStr << endl;
        hasError = true;
    }

    int flightNumber = 0;
    int passengers = 0;
    int delayMinutes = 0;

    if (!hasError) {
        flightNumber = atoi(flightStr);
        passengers = atoi(passengersStr);

        if (flightNumber <= 0) {
            cout << "Line " << lineNumber << ": Error: flight number must be positive." << endl;
            hasError = true;
        }

        if (passengers < 0 || passengers > 1000) {
            cout << "Line " << lineNumber << ": Error: invalid number of passengers." << endl;
            hasError = true;
        }

        if (!parseDelay(delayStr, delayMinutes)) {
            cout << "Line " << lineNumber << ": Error: invalid delay time: " << delayStr << endl;
            hasError = true;
        }
    }
    else {
        parseDelay(delayStr, delayMinutes);
    }

    if (hasError) {
        return false;
    }

    plane.flightNumber = flightNumber;
    strcpy(plane.boardNumber, boardStr);
    plane.passengers = passengers;
    plane.delayMinutes = delayMinutes;

    return true;
}

void selectionSortIndexes(const Plane planes[], int indexes[], int count) {
    for (int i = 0; i < count - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < count; j++) {
            if (planes[indexes[j]].delayMinutes < planes[indexes[minIndex]].delayMinutes) {
                minIndex = j;
            }
        }
        int temp = indexes[i];
        indexes[i] = indexes[minIndex];
        indexes[minIndex] = temp;
    }
}

void printTable(const Plane planes[], const int indexes[], int count) {
    cout << left
        << setw(15) << "Flight Number"
        << setw(18) << "Board Number"
        << setw(18) << "Passengers"
        << setw(15) << "Delay"
        << endl;

    cout << "------------------------------------------------------------------" << endl;

    for (int i = 0; i < count; i++) {
        int index = indexes[i];
        char delayText[MAX_FIELD_LEN];
        delayToString(planes[index].delayMinutes, delayText);

        cout << left
            << setw(15) << planes[index].flightNumber
            << setw(18) << planes[index].boardNumber
            << setw(18) << planes[index].passengers
            << setw(15) << delayText
            << endl;
    }
}