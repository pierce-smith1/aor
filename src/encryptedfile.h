#pragma once

#include <QFile>
#include <QRandomGenerator>

const static int ENCRYPTION_SEED = 2270;

class EncryptedFile : public QFile {
public:
    EncryptedFile(const QString &path);

    bool reset() override;
    bool seek(qint64 pos) override;

protected:
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;

private:
    QRandomGenerator m_rng;
};
