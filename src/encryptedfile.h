#pragma once

#include <QFile>
#include <QRandomGenerator>

#include "types.h"

const static int ENCRYPTION_SEED = 2270;

class EncryptedFile : public QFile {
public:
    EncryptedFile(const QString &path);

    bool reset() override;
    bool seek(AorInt pos) override;

protected:
    AorInt readData(char *data, AorInt maxSize) override;
    AorInt writeData(const char *data, AorInt maxSize) override;

private:
    QRandomGenerator m_rng;
};
