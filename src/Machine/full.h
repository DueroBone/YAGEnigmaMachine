#ifndef FULL_H
#define FULL_H

#ifdef __cplusplus
extern "C" {
#endif
    void runTest(char* inputStr, size_t length);
    void encrypt(int rotorNumbers[3], char reflectorNumber, int ringSettings[3], int rotorPositions[3], const uint8_t *input, size_t length, uint8_t *output);

#ifdef __cplusplus
}
#endif

#endif