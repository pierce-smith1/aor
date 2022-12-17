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

bool EncryptedFile::seek(qint64 pos) {
    bool success = QFile::seek(pos);

    if (success) {
        for (qint64 i = 0; i < pos; i++) {
            m_rng.generate();
        }
    }

    return success;
}

qint64 EncryptedFile::readData(char *data, qint64 maxSize) {
    char *intermediate_data = new char[maxSize];

    qint64 ret = QFile::readData(intermediate_data, maxSize);
    for (qint64 i = 0; i < ret; i++) {
        data[i] = intermediate_data[i] ^ (m_rng.generate() & 0xff);
    }

    delete[] intermediate_data;
    return ret;
}

qint64 EncryptedFile::writeData(const char *data, qint64 maxSize) {
    char *encrypted_data = new char[maxSize];

    for (qint64 i = 0; i < maxSize; i++) {
        encrypted_data[i] = data[i] ^ (m_rng.generate() & 0xff);
    }

    qint64 ret = QFile::writeData(encrypted_data, maxSize);

    delete[] encrypted_data;
    return ret;
}
