inline void _waitForIdleBus() { while (I2C1CON & 0x1f) {} }

void DS3231::begin()
{
	if ((_sda_pin == SDA) and (_scl_pin == SCL))
	{
		uint32_t	tpgd;

		_use_hw = true;
		pinMode(SDA, OUTPUT);
		digitalWrite(SDA, HIGH);
		IFS0CLR = 0xE0000000;									// Clear Interrupt Flag
		IEC0CLR = 0xE0000000;									// Disable Interrupt
		I2C1CONCLR = (1 << _I2CCON_ON);							// Disable I2C interface
		tpgd = ((F_CPU / 8) * 104) / 125000000;
		I2C1BRG = (F_CPU / (2 * TWI_FREQ) - tpgd) - 2;			// Set I2C Speed
		I2C1ADD = DS3231_ADDR;									// Set I2C device address
		I2C1CONSET = (1 << _I2CCON_ON) | (1 << _I2CCON_STREN);	// Enable I2C Interface
	}
	else
	{
		_use_hw = false;
		pinMode(_scl_pin, OUTPUT);
	}
}

void DS3231::_burstRead()
{
	if (_use_hw)
	{
		_waitForIdleBus();									// Wait for I2C bus to be Idle before starting
		I2C1CONSET = (1 << _I2CCON_SEN);					// Send start condition
		if (I2C1STAT & (1 << _I2CSTAT_BCL)) { return; }		// Check if there is a bus collision
		while (I2C1CON & (1 << _I2CCON_SEN)) {}				// Wait for start condition to finish
		I2C1TRN = (DS3231_ADDR<<1);							// Send device Write address
		while (I2C1STAT & (1 << _I2CSTAT_IWCOL))			// Check if there is a Write collision
		{
			I2C1STATCLR = (1 << _I2CSTAT_IWCOL);			// Clear Write collision flag
			I2C1TRN = (DS3231_ADDR<<1);						// Retry send device Write address
		}
		while (I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}		// Wait for transmit to finish
		while (I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}		// Wait for ACK
		I2C1TRN = 0;										// Send the register address
		while (I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}		// Wait for transmit to finish
		while (I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}		// Wait for ACK
		_waitForIdleBus();									// Wait for I2C bus to be Idle before starting
		I2C1CONSET = (1 << _I2CCON_RSEN);					// Send start condition
		if (I2C1STAT & (1 << _I2CSTAT_BCL)) { return; }		// Check if there is a bus collision
		while (I2C1CON & (1 << _I2CCON_RSEN)) {}			// Wait for start condition to finish
		I2C1TRN = (DS3231_ADDR<<1) | 1;						// Send device Read address
		while (I2C1STAT & (1 << _I2CSTAT_IWCOL))			// Check if there is a Write collision
		{
			I2C1STATCLR = (1 << _I2CSTAT_IWCOL);			// Clear Write collision flag
			I2C1TRN = (DS3231_ADDR<<1) | 1;					// Retry send device Read address
		}
		while (I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}		// Wait for transmit to finish
		while (I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}		// Wait for ACK
		byte dummy = I2C1RCV;								// Clear _I2CSTAT_RBF (Receive Buffer Full)
		for (int i=0; i<7; i++)
		{
			_waitForIdleBus();								// Wait for I2C bus to be Idle before continuing
			I2C1CONSET = (1 << _I2CCON_RCEN);				// Set RCEN to start receive
			while (I2C1CON & (1 << _I2CCON_RCEN)) {}		// Wait for Receive operation to finish
			while (!(I2C1STAT & (1 << _I2CSTAT_RBF))) {}	// Wait for Receive Buffer Full
			_burstArray[i] = I2C1RCV;						// Read data
			if (i == 6)
				I2C1CONSET = (1 << _I2CCON_ACKDT);			// Prepare to send NACK
			else
				I2C1CONCLR = (1 << _I2CCON_ACKDT);			// Prepare to send ACK
			I2C1CONSET = (1 << _I2CCON_ACKEN);				// Send ACK/NACK
			while (I2C1CON & (1 << _I2CCON_ACKEN)) {}		// Wait for ACK/NACK send to finish
		}
		I2C1CONSET = (1 << _I2CCON_PEN);					// Send stop condition
		while (I2C1CON & (1 << _I2CCON_PEN)) {}				// Wait for stop condition to finish
	}
	else
	{
		_sendStart(DS3231_ADDR_W);
		_waitForAck();
		_writeByte(0);
		_waitForAck();
		_sendStart(DS3231_ADDR_R);
		_waitForAck();

		for (int i=0; i<7; i++)
		{
			_burstArray[i] = _readByte();
			if (i<6)
				_sendAck();
			else
				_sendNack();
		}
		_sendStop();
	}
}

uint8_t DS3231::_readRegister(uint8_t reg)
{
	uint8_t	readValue=0;

	if (_use_hw)
	{
		_waitForIdleBus();									// Wait for I2C bus to be Idle before starting
		I2C1CONSET = (1 << _I2CCON_SEN);					// Send start condition
		if (I2C1STAT & (1 << _I2CSTAT_BCL)) { return 0xff; }// Check if there is a bus collision
		while (I2C1CON & (1 << _I2CCON_SEN)) {}				// Wait for start condition to finish
		I2C1TRN = (DS3231_ADDR<<1);							// Send device Write address
		while (I2C1STAT & (1 << _I2CSTAT_IWCOL))			// Check if there is a Write collision
		{
			I2C1STATCLR = (1 << _I2CSTAT_IWCOL);			// Clear Write collision flag
			I2C1TRN = (DS3231_ADDR<<1);						// Retry send device Write address
		}
		while (I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}		// Wait for transmit to finish
		while (I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}		// Wait for ACK
		I2C1TRN = reg;										// Send the register address
		while (I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}		// Wait for transmit to finish
		while (I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}		// Wait for ACK
		_waitForIdleBus();									// Wait for I2C bus to be Idle before starting
		I2C1CONSET = (1 << _I2CCON_RSEN);					// Send start condition
		if (I2C1STAT & (1 << _I2CSTAT_BCL)) { return 0xff; }// Check if there is a bus collision
		while (I2C1CON & (1 << _I2CCON_RSEN)) {}			// Wait for start condition to finish
		I2C1TRN = (DS3231_ADDR<<1) | 1;						// Send device Read address
		while (I2C1STAT & (1 << _I2CSTAT_IWCOL))			// Check if there is a Write collision
		{
			I2C1STATCLR = (1 << _I2CSTAT_IWCOL);			// Clear Write collision flag
			I2C1TRN = (DS3231_ADDR<<1) | 1;					// Retry send device Read address
		}
		while (I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}		// Wait for transmit to finish
		while (I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}		// Wait for ACK
		byte dummy = I2C1RCV;								// Clear _I2CSTAT_RBF (Receive Buffer Full)
		_waitForIdleBus();									// Wait for I2C bus to be Idle before continuing
		I2C1CONSET = (1 << _I2CCON_RCEN);					// Set RCEN to start receive
		while (I2C1CON & (1 << _I2CCON_RCEN)) {}			// Wait for Receive operation to finish
		while (!(I2C1STAT & (1 << _I2CSTAT_RBF))) {}		// Wait for Receive Buffer Full
		readValue = I2C1RCV;								// Read data
		I2C1CONSET = (1 << _I2CCON_ACKDT);					// Prepare to send NACK
		I2C1CONSET = (1 << _I2CCON_ACKEN);					// Send NACK
		while (I2C1CON & (1 << _I2CCON_ACKEN)) {}			// Wait for NACK send to finish
		I2C1CONSET = (1 << _I2CCON_PEN);					// Send stop condition
		while (I2C1CON & (1 << _I2CCON_PEN)) {}				// Wait for stop condition to finish
	}
	else
	{
		_sendStart(DS3231_ADDR_W);
		_waitForAck();
		_writeByte(reg);
		_waitForAck();
		_sendStart(DS3231_ADDR_R);
		_waitForAck();
		readValue = _readByte();
		_sendNack();
		_sendStop();
	}
	return readValue;
}

void DS3231::_writeRegister(uint8_t reg, uint8_t value)
{
	if (_use_hw)
	{
		_waitForIdleBus();									// Wait for I2C bus to be Idle before starting
		I2C1CONSET = (1 << _I2CCON_SEN);					// Send start condition
		if (I2C1STAT & (1 << _I2CSTAT_BCL)) { return; }		// Check if there is a bus collision
		while (I2C1CON & (1 << _I2CCON_SEN)) {}				// Wait for start condition to finish
		I2C1TRN = (DS3231_ADDR<<1);							// Send device Write address
		while (I2C1STAT & (1 << _I2CSTAT_IWCOL))			// Check if there is a Write collision
		{
			I2C1STATCLR = (1 << _I2CSTAT_IWCOL);			// Clear Write collision flag
			I2C1TRN = (DS3231_ADDR<<1);						// Retry send device Write address
		}
		while (I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}		// Wait for transmit to finish
		while (I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}		// Wait for ACK
		I2C1TRN = reg;										// Send the 1st data byte
		while (I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}		// Wait for transmit to finish
		while (I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}		// Wait for ACK
		I2C1TRN = value;									// Send the 2nd data byte
		while (I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}		// Wait for transmit to finish
		while (I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}		// Wait for ACK
		I2C1CONSET = (1 << _I2CCON_PEN);					// Send stop condition
		while (I2C1CON & (1 << _I2CCON_PEN)) {}				// Wait for stop condition to finish
	}
	else
	{
		_sendStart(DS3231_ADDR_W);
		_waitForAck();
		_writeByte(reg);
		_waitForAck();
		_writeByte(value);
		_waitForAck();
		_sendStop();
	}
}
