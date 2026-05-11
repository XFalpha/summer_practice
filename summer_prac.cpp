#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <cstdio>

using namespace std;

const int MAX_PLANES = 100;
const int MAX_BOARD_LEN = 20;
const int MAX_FILE_NAME = 100;
const int MAX_FIELD_LEN = 50;

struct Plane {
    int flightNumber;
    char boardNumber[MAX_BOARD_LEN];
    int passengers;
    int delayMinutes;
};

bool isNumber(const char s[]) {
    if (s[0] == '\0') return false;

    for (int i = 0; s[i] != '\0'; i++) {
        if (!isdigit(static_cast<unsigned char>(s[i]))) {
            return false;
        }
    }

    return true;
}

bool isRussianB(const char s[]) {
    unsigned char c1 = static_cast<unsigned char>(s[0]);
    unsigned char c2 = static_cast<unsigned char>(s[1]);

    return (c1 == 0xD0 && (c2 == 0x91 || c2 == 0xB1));
}

bool isValidBoardNumber(const char s[]) {
    if (s[0] == '\0') return false;

    int startDigits = 0;

    if ((s[0] == 'B' || s[0] == 'b') && s[1] == '-') {
        startDigits = 2;
    }
    else if (isRussianB(s) && s[2] == '-') {
        startDigits = 3;
    }
    else {
        return false;
    }

    if (s[startDigits] == '\0') return false;

    for (int i = startDigits; s[i] != '\0'; i++) {
        if (!isdigit(static_cast<unsigned char>(s[i]))) {
            return false;
        }
    }

    return true;
}

bool parseDelay(const char s[], int& totalMinutes) {
    if (s[0] == '\0') return false;

    char temp[MAX_FIELD_LEN];
    strcpy_s(temp, s);

    char* context = nullptr;

    char* part1 = strtok_s(temp, ":", &context);
    char* part2 = strtok_s(NULL, ":", &context);
    char* part3 = strtok_s(NULL, ":", &context);
    char* extra = strtok_s(NULL, ":", &context);

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

    sprintf_s(result, MAX_FIELD_LEN, "%d:%02d:%02d", days, hours, minutes);
}

bool readPlane(ifstream& fin, Plane& plane) {
    char flightStr[MAX_FIELD_LEN];
    char boardStr[MAX_BOARD_LEN];
    char passengersStr[MAX_FIELD_LEN];
    char delayStr[MAX_FIELD_LEN];

    if (!(fin >> flightStr >> boardStr >> passengersStr >> delayStr)) {
        return false;
    }

    if (!isNumber(flightStr)) {
        cout << "Ошибка: некорректный номер рейса: " << flightStr << endl;
        return false;
    }

    if (!isValidBoardNumber(boardStr)) {
        cout << "Ошибка: некорректный бортовой номер: " << boardStr << endl;
        return false;
    }

    if (!isNumber(passengersStr)) {
        cout << "Ошибка: некорректное количество пассажиров: " << passengersStr << endl;
        return false;
    }

    int flightNumber = atoi(flightStr);
    int passengers = atoi(passengersStr);
    int delayMinutes = 0;

    if (flightNumber <= 0) {
        cout << "Ошибка: номер рейса должен быть положительным." << endl;
        return false;
    }

    if (passengers < 0 || passengers > 1000) {
        cout << "Ошибка: некорректное количество пассажиров." << endl;
        return false;
    }

    if (!parseDelay(delayStr, delayMinutes)) {
        cout << "Ошибка: некорректное время опоздания: " << delayStr << endl;
        return false;
    }

    plane.flightNumber = flightNumber;

    strcpy_s(plane.boardNumber, boardStr);

    plane.passengers = passengers;
    plane.delayMinutes = delayMinutes;

    return true;
}

void selectionSortIndexes(const Plane planes[], int indexes[], int count) {
    for (int i = 0; i < count - 1; i++) {
        int minIndex = i;

        for (int j = i + 1; j < count; j++) {
            if (planes[indexes[j]].delayMinutes <
                planes[indexes[minIndex]].delayMinutes) {

                minIndex = j;
            }
        }

        int temp = indexes[i];
        indexes[i] = indexes[minIndex];
        indexes[minIndex] = temp;
    }
}

void printTable(const Plane planes[],
    const int indexes[],
    int count) {

    cout << left
        << setw(15) << "Номер рейса"
        << setw(18) << "Бортовой номер"
        << setw(18) << "Пассажиры"
        << setw(15) << "Опоздание"
        << endl;

    cout << "------------------------------------------------------------------"
        << endl;

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

int main() {

    setlocale(LC_ALL, "Russian");

    char fileName[MAX_FILE_NAME];

    cout << "Введите имя файла с данными: ";

    cin.getline(fileName, MAX_FILE_NAME);

    if (fileName[0] == '\0') {

        cout << "Ошибка: имя файла не может быть пустым." << endl;

        return 1;
    }

    ifstream fin(fileName);

    if (!fin.is_open()) {

        cout << "Ошибка: не удалось открыть файл." << endl;

        return 1;
    }

    Plane planes[MAX_PLANES];

    int count = 0;

    while (count < MAX_PLANES &&
        readPlane(fin, planes[count])) {

        count++;
    }

    if (!fin.eof()) {

        cout << "Чтение остановлено из-за ошибки "
            << "в файле или превышения лимита записей."
            << endl;

        return 1;
    }

    fin.close();

    if (count == 0) {

        cout << "Ошибка: файл пуст "
            << "или не содержит корректных данных."
            << endl;

        return 1;
    }

    int indexes[MAX_PLANES];

    for (int i = 0; i < count; i++) {
        indexes[i] = i;
    }

    selectionSortIndexes(planes, indexes, count);

    cout << endl
        << "Отсортированная таблица "
        << "по времени опоздания:"
        << endl;

    printTable(planes, indexes, count);

    return 0;
}


