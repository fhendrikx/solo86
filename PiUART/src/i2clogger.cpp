#include "i2clogger.h"

CI2CLogger::CI2CLogger(CI2CMaster *pI2C) {
    m_pI2C = pI2C;
}

CI2CLogger::~CI2CLogger() {}

int CI2CLogger::Write(const void *pBuffer, size_t nCount) {

    int ret = m_pI2C->Write(I2C_LOGGER_ADDR, pBuffer, nCount);
    
    if (ret > 0)
	return ret;
    else
	return 0;
    
}
