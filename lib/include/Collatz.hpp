#pragma once

int collatzNumber(int startNumber) {
    int sequenceNumber = 0;

    int currentNumber = startNumber;

    while (currentNumber != 1) {
        if (currentNumber % 2 == 0) {
            currentNumber = currentNumber / 2;
        } else {
            currentNumber = 3*currentNumber + 1;
        }

        sequenceNumber++;
    }

    return sequenceNumber;
}