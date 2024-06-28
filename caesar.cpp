#include <iostream>
#include <cstring>
#include "caesar.h"

    char* encrypt(const char* rawText, int key) {
        int length = strlen(rawText);
        char* encryptedText = new char[length + 1];

        key = key % 26;

        for (int i = 0; i < length; ++i) {
            char ch = rawText[i];

            if (ch >= 'A' && ch <= 'Z') {
                encryptedText[i] = 'A' + (ch - 'A' + key) % 26;
            }
            else if (ch >= 'a' && ch <= 'z') {
                encryptedText[i] = 'a' + (ch - 'a' + key) % 26;
            }
            else {
                encryptedText[i] = ch;
            }
        }
        encryptedText[length] = '\0';

        return encryptedText;
    }

char* decryptFunc(const char* text, int key) {
        // Assuming this function implements the decryption logic
        int length = std::strlen(text);
        char* decryptedText = new char[length + 1];

        key = key % 26;

        for (int i = 0; i < length; ++i) {
            char ch = text[i];

            if (ch >= 'a' && ch <= 'z') {
                decryptedText[i] = 'a' + (ch - 'a' - key + 26) % 26;
            } else if (ch >= 'A' && ch <= 'Z') {
                decryptedText[i] = 'A' + (ch - 'A' - key + 26) % 26;
            } else {
                decryptedText[i] = ch;
            }
        }

        decryptedText[length] = '\0';
        return decryptedText;
    }

char* decrypt(const char* text, int key) {
        return decryptFunc(text, key);
    }

