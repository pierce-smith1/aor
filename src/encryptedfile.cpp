// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "encryptedfile.h"

EncryptedFile::EncryptedFile(const QString &path)
    : QFile(path)
{
    m_rng.seed(ENCRYPTION_SEED);
}

bool EncryptedFile::reset() {
    m_rng.seed(ENCRYPTION_SEED);
    return QFile::reset();
}

bool EncryptedFile::seek(AorInt pos) {
    bool success = QFile::seek(pos);

    if (success) {
        for (AorInt i = 0; i < pos; i++) {
            m_rng.generate();
        }
    }

    return success;
}

AorInt EncryptedFile::readData(char *data, AorInt maxSize) {
    char *intermediate_data = new char[maxSize];

    AorInt ret = QFile::readData(intermediate_data, maxSize);
    for (AorInt i = 0; i < ret; i++) {
        data[i] = intermediate_data[i] ^ (m_rng.generate() & 0xff);
    }

    delete[] intermediate_data;
    return ret;
}

AorInt EncryptedFile::writeData(const char *data, AorInt maxSize) {
    char *encrypted_data = new char[maxSize];

    for (AorInt i = 0; i < maxSize; i++) {
        encrypted_data[i] = data[i] ^ (m_rng.generate() & 0xff);
    }

    AorInt ret = QFile::writeData(encrypted_data, maxSize);

    delete[] encrypted_data;
    return ret;
}
