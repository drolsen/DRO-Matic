void DS3231::begin()
{
	_use_hw = false;
	if ((_sda_pin == SDA) and (_scl_pin == SCL))
	{
		_use_hw = true;
		twi = TWI1;
		pmc_enable_periph_clk(WIRE_INTERFACE_ID);
		PIO_Configure(g_APinDescription[PIN_WIRE_SDA].pPort, g_APinDescription[PIN_WIRE_SDA].ulPinType, g_APinDescription[PIN_WIRE_SDA].ulPin, g_APinDescription[PIN_WIRE_SDA].ulPinConfiguration);
		PIO_Configure(g_APinDescription[PIN_WIRE_SCL].pPort, g_APinDescription[PIN_WIRE_SCL].ulPinType, g_APinDescription[PIN_WIRE_SCL].ulPin, g_APinDescription[PIN_WIRE_SCL].ulPinConfiguration);
		NVIC_DisableIRQ(TWI1_IRQn);
		NVIC_ClearPendingIRQ(TWI1_IRQn);
		NVIC_SetPriority(TWI1_IRQn, 0);
		NVIC_EnableIRQ(TWI1_IRQn);

	}
	else if ((_sda_pin == SDA1) and (_scl_pin == SCL1))
	{
		_use_hw = true;
		twi = TWI0;
		pmc_enable_periph_clk(WIRE1_INTERFACE_ID);
		PIO_Configure(g_APinDescription[PIN_WIRE1_SDA].pPort, g_APinDescription[PIN_WIRE1_SDA].ulPinType, g_APinDescription[PIN_WIRE1_SDA].ulPin, g_APinDescription[PIN_WIRE1_SDA].ulPinConfiguration);
		PIO_Configure(g_APinDescription[PIN_WIRE1_SCL].pPort, g_APinDescription[PIN_WIRE1_SCL].ulPinType, g_APinDescription[PIN_WIRE1_SCL].ulPin, g_APinDescription[PIN_WIRE1_SCL].ulPinConfiguration);
		NVIC_DisableIRQ(TWI0_IRQn);
		NVIC_ClearPendingIRQ(TWI0_IRQn);
		NVIC_SetPriority(TWI0_IRQn, 0);
		NVIC_EnableIRQ(TWI0_IRQn);
	}

	if (_use_hw)
	{
		// activate internal pullups for twi.
		digitalWrite(SDA, 1);
		digitalWrite(SCL, 1);

		// Reset the TWI
		twi->TWI_CR = TWI_CR_SWRST;
		// TWI Slave Mode Disabled, TWI Master Mode Disabled.
		twi->TWI_CR = TWI_CR_SVDIS;
		twi->TWI_CR = TWI_CR_MSDIS;
		// Set TWI Speed
		twi->TWI_CWGR = (TWI_DIV << 16) | (TWI_SPEED << 8) | TWI_SPEED;
		// Set master mode
		twi->TWI_CR = TWI_CR_MSEN;
	}
	else
	{
		pinMode(_scl_pin, OUTPUT);
	}
}

void DS3231::_burstRead()
{
	if (_use_hw)
	{
		// Set slave address and number of internal address bytes.
		twi->TWI_MMR = (1 << 8) | TWI_MMR_MREAD | (DS3231_ADDR << 16);
		// Set internal address bytes
		twi->TWI_IADR = 0;
		// Send START condition
		twi->TWI_CR = TWI_CR_START;

		for (int i=0; i<6; i++)
		{
			while ((twi->TWI_SR & TWI_SR_RXRDY) != TWI_SR_RXRDY)
			{
			};
			_burstArray[i] = twi->TWI_RHR;
		}

		twi->TWI_CR = TWI_CR_STOP;
		while ((twi->TWI_SR & TWI_SR_RXRDY) != TWI_SR_RXRDY) {};
		_burstArray[6] = twi->TWI_RHR;
		while ((twi->TWI_SR & TWI_SR_TXCOMP) != TWI_SR_TXCOMP) {};
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
		// Set slave address and number of internal address bytes.
		twi->TWI_MMR = (1 << 8) | TWI_MMR_MREAD | (DS3231_ADDR << 16);
		// Set internal address bytes
		twi->TWI_IADR = reg;
		// Send START and STOP condition to read a single byte
		twi->TWI_CR = TWI_CR_START | TWI_CR_STOP;
		while ((twi->TWI_SR & TWI_SR_RXRDY) != TWI_SR_RXRDY) {};
		readValue = twi->TWI_RHR;
		while ((twi->TWI_SR & TWI_SR_TXCOMP) != TWI_SR_TXCOMP) {};
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
		// Set slave address and number of internal address bytes.
		twi->TWI_MMR = (1 << 8) | (DS3231_ADDR << 16);
		// Set internal address bytes
		twi->TWI_IADR = reg;
		// Send a single byte to start transfer
		twi->TWI_THR = value;
		while ((twi->TWI_SR & TWI_SR_TXRDY) != TWI_SR_TXRDY) {};
		// Send STOP condition
		twi->TWI_CR = TWI_CR_STOP;
		while ((twi->TWI_SR & TWI_SR_TXCOMP) != TWI_SR_TXCOMP) {};
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

