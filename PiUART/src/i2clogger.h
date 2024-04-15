#ifndef I2C_LOGGER_H
#define I2C_LOGGER_H

#include <circle/device.h>
#include <circle/i2cmaster.h>

#define I2C_LOGGER_ADDR 0x7f

class CI2CLogger : public CDevice {
public:

    CI2CLogger(CI2CMaster *pI2C);
    ~CI2CLogger();

    int Write(const void *pBuffer, size_t nCount);

private:

    CI2CMaster *m_pI2C;

};

#endif

